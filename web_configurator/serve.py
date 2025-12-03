#!/usr/bin/env python3
"""
Flask-based backend for the Axis Driver Web Configurator.

Replaces the previous SimpleHTTPRequestHandler server with a Flask app that:
- Serves the files in the web_configurator directory as static files
- Exposes POST /udp/send to send UDP packets (payload base64)
- Exposes GET /udp/stream as an SSE endpoint delivering incoming UDP packets

This file tries to preserve the behaviour of the prior script but uses Flask
for easier extensibility.
"""

import sys
import time
import socket
import threading
import base64
import json
from pathlib import Path
import queue

try:
    from flask import Flask, request, send_from_directory, Response, stream_with_context, jsonify
except Exception:
    print("Flask is required. Install with: pip install flask")
    sys.exit(1)

# Directory containing the web UI (this file's parent)
WEB_DIR = Path(__file__).parent
PORT = 8081

# UDP and SSE state
udp_socket = None
udp_lock = threading.Lock()

# For SSE we maintain a list of queues (one per connected client). The UDP
# receiver thread will put JSON payload strings into each queue.
sse_clients = []
sse_clients_lock = threading.Lock()

app = Flask(__name__, static_folder=str(WEB_DIR), static_url_path="")


def udp_receiver_loop(sock):
    """Receive UDP packets and forward them to all SSE client queues as base64 JSON."""
    while True:
        try:
            data, addr = sock.recvfrom(65536)
            if not data:
                continue
            b64 = base64.b64encode(data).decode('ascii')
            payload = json.dumps({'from': addr[0], 'port': addr[1], 'payload': b64})

            with sse_clients_lock:
                for q in list(sse_clients):
                    try:
                        # Put the payload string into each client's queue (non-blocking)
                        q.put_nowait(payload)
                    except Exception:
                        # If queue is full or other error, drop this client later
                        pass
        except Exception:
            # Avoid tight-looping on persistent errors
            time.sleep(0.1)


@app.route('/udp/send', methods=['POST'])
def udp_send():
    """POST /udp/send -> { ip, port, payload: base64 }
    Sends the provided payload to (ip, port) using a shared UDP socket.
    """
    data = None
    try:
        data = request.get_json(force=True)
    except Exception:
        return "Invalid JSON", 400

    if not data:
        return "Invalid JSON", 400

    ip = data.get('ip')
    port = data.get('port')
    payload_b64 = data.get('payload')

    try:
        port = int(port)
    except Exception:
        return "Invalid port", 400

    try:
        payload = base64.b64decode(payload_b64) if payload_b64 else b''
    except Exception:
        return "Invalid base64 payload", 400

    global udp_socket
    with udp_lock:
        if udp_socket is None:
            udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            udp_socket.bind(('', 0))
            t = threading.Thread(target=udp_receiver_loop, args=(udp_socket,), daemon=True)
            t.start()

        try:
            udp_socket.sendto(payload, (ip, port))
        except Exception as e:
            return str(e), 500

    return jsonify({'status': 'ok'})


def sse_stream(client_queue):
    """Generator that yields Server-Sent Events from the given queue."""
    try:
        while True:
            # Wait for next payload (blocking)
            payload = client_queue.get()
            yield f"data: {payload}\n\n"
    except GeneratorExit:
        # Client disconnected
        return
    finally:
        # ensure the client's queue is removed
        with sse_clients_lock:
            try:
                sse_clients.remove(client_queue)
            except ValueError:
                pass


@app.route('/udp/stream')
def udp_stream():
    """SSE endpoint that streams incoming UDP packets to the browser."""
    q = queue.Queue()
    with sse_clients_lock:
        sse_clients.append(q)

    # Use stream_with_context to ensure the Flask request context stays alive
    return Response(stream_with_context(sse_stream(q)), mimetype='text/event-stream')


@app.route('/', defaults={'path': 'index.html'})
@app.route('/<path:path>')
def static_proxy(path):
    # Serve static files from the WEB_DIR
    return send_from_directory(str(WEB_DIR), path)


def main():
    try:
        print(f"🚀 Axis Driver Web Configurator (Flask)")
        print(f"📡 Serving at: http://localhost:{PORT}")
        print(f"📁 Directory: {WEB_DIR}")
        print("⚡ Press Ctrl+C to stop the server")
        print("-" * 60)
        app.run(host='0.0.0.0', port=PORT, threaded=True)
    except OSError as e:
        if getattr(e, 'errno', None) == 98:
            print(f"❌ Port {PORT} is already in use. Try a different port or stop the existing server.")
            sys.exit(1)
        else:
            print(f"❌ Error starting server: {e}")
            sys.exit(1)
    except KeyboardInterrupt:
        print("\n🛑 Server stopped by user")


if __name__ == '__main__':
    main()
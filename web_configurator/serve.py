#!/usr/bin/env python3
"""
Simple HTTP server for the Axis Driver Web Configurator.
This serves the files over HTTP which is required for Web Serial API to work.
"""

import http.server
import socketserver
import os
import sys
from pathlib import Path
import threading
import socket
import json
import base64
import time

# Set the directory to serve
WEB_DIR = Path(__file__).parent
PORT = 8081

# UDP forwarding state
udp_socket = None
udp_lock = threading.Lock()
# SSE clients: list of handler objects (instances of CustomHTTPRequestHandler)
sse_clients = []
sse_clients_lock = threading.Lock()

def udp_receiver_loop(sock):
    """Receive UDP packets on the given socket and forward them to SSE clients as base64 strings."""
    while True:
        try:
            data, addr = sock.recvfrom(65536)
            if not data:
                continue
            b64 = base64.b64encode(data).decode('ascii')
            payload = json.dumps({'from': addr[0], 'port': addr[1], 'payload': b64})

            # Send to all SSE clients
            with sse_clients_lock:
                for client in list(sse_clients):
                    try:
                        # SSE: data: <line>\n\n
                        client.wfile.write(f"data: {payload}\n\n".encode('utf-8'))
                        client.wfile.flush()
                    except Exception:
                        try:
                            sse_clients.remove(client)
                        except ValueError:
                            pass
        except Exception:
            # sleep briefly to avoid busy-looping on persistent errors
            time.sleep(0.1)

class CustomHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(WEB_DIR), **kwargs)
    
    def end_headers(self):
        # Add headers for better compatibility
        self.send_header('Cache-Control', 'no-cache, no-store, must-revalidate')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Expires', '0')
        super().end_headers()

    def do_POST(self):
        """
        POST /udp/send  -> { ip, port, payload: base64 }
        The server will send the payload via UDP to (ip,port) using a shared UDP socket.
        """
        if self.path == '/udp/send':
            length = int(self.headers.get('Content-Length', 0))
            body = self.rfile.read(length) if length else b''
            try:
                data = json.loads(body.decode('utf-8'))
                ip = data.get('ip')
                port = int(data.get('port'))
                payload_b64 = data.get('payload')
                payload = base64.b64decode(payload_b64) if payload_b64 else b''
            except Exception as e:
                self.send_response(400)
                self.end_headers()
                self.wfile.write(b'Invalid JSON')
                return

            # Ensure UDP socket exists and is bound
            global udp_socket
            with udp_lock:
                if udp_socket is None:
                    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                    # Bind to an OS-assigned port so replies come back here
                    udp_socket.bind(('', 0))
                    # Start receiver thread
                    t = threading.Thread(target=udp_receiver_loop, args=(udp_socket,), daemon=True)
                    t.start()

            try:
                with udp_lock:
                    udp_socket.sendto(payload, (ip, port))
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.end_headers()
                resp = {'status': 'ok'}
                self.wfile.write(json.dumps(resp).encode('utf-8'))
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(str(e).encode('utf-8'))
            return

        # Fallback to default behaviour for other POSTs
        return super().do_POST()

    def do_GET(self):
        # SSE stream for incoming UDP packets
        if self.path == '/udp/stream':
            self.send_response(200)
            self.send_header('Content-Type', 'text/event-stream')
            self.send_header('Cache-Control', 'no-cache')
            self.send_header('Connection', 'keep-alive')
            self.end_headers()

            # Register client
            with sse_clients_lock:
                sse_clients.append(self)

            try:
                # Keep the connection open. The udp_receiver_loop will write to self.wfile when data arrives.
                while True:
                    time.sleep(0.5)
                    # If wfile is closed, break
                    if getattr(self, 'wfile', None) is None:
                        break
            except Exception:
                pass
            finally:
                with sse_clients_lock:
                    try:
                        sse_clients.remove(self)
                    except ValueError:
                        pass
            return

        return super().do_GET()


def main():
    try:
        # Use a threading server so SSE connections and POST handlers can run concurrently
        with http.server.ThreadingHTTPServer(("", PORT), CustomHTTPRequestHandler) as httpd:
            print(f"🚀 Axis Driver Web Configurator Server")
            print(f"📡 Serving at: http://localhost:{PORT}")
            print(f"📁 Directory: {WEB_DIR}")
            print(f"🌐 Open your browser to: http://localhost:{PORT}")
            print(f"🔌 Make sure your Axis Driver is connected via USB")
            print(f"⚡ Press Ctrl+C to stop the server")
            print("-" * 60)
            
            httpd.serve_forever()
            
    except KeyboardInterrupt:
        print("\n🛑 Server stopped by user")
    except OSError as e:
        if e.errno == 98:  # Address already in use
            print(f"❌ Port {PORT} is already in use. Try a different port or stop the existing server.")
            sys.exit(1)
        else:
            print(f"❌ Error starting server: {e}")
            sys.exit(1)

if __name__ == "__main__":
    main()
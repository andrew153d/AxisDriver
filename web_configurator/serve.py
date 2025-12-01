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

# Set the directory to serve
WEB_DIR = Path(__file__).parent
PORT = 8081

class CustomHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(WEB_DIR), **kwargs)
    
    def end_headers(self):
        # Add headers for better compatibility
        self.send_header('Cache-Control', 'no-cache, no-store, must-revalidate')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Expires', '0')
        super().end_headers()

def main():
    try:
        with socketserver.TCPServer(("", PORT), CustomHTTPRequestHandler) as httpd:
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
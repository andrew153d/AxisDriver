#!/bin/bash

# Axis Driver Web Configurator Launcher
# This script starts a local HTTP server for the web configurator

echo "🚀 Starting Axis Driver Web Configurator..."
echo "⚡ Make sure your Axis Driver is connected via USB"
echo ""

cd "$(dirname "$0")"

# Check if Python 3 is available
if command -v python3 &> /dev/null; then
    python3 serve.py
elif command -v python &> /dev/null; then
    python serve.py
else
    echo "❌ Python is not installed or not in PATH"
    echo "Please install Python 3 to run the web configurator"
    exit 1
fi
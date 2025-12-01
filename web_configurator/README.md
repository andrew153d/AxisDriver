# Axis Driver Web Configurator

A retro computing-themed web interface for configuring and controlling the Axis Driver motor control system.

## 🚀 Quick Start

### Method 1: Use the startup script (Recommended)
```bash
cd web_configurator
./start_server.sh
```

### Method 2: Run Python server directly
```bash
cd web_configurator
python3 serve.py
```

### Method 3: Use any HTTP server
```bash
cd web_configurator
# Python built-in server
python3 -m http.server 8000

# Or Node.js if you have it
npx http-server -p 8000

# Or PHP if you have it
php -S localhost:8000
```

## 🌐 Accessing the Interface

1. Start the server using one of the methods above
2. Open your web browser to: **http://localhost:8000**
3. Connect your Axis Driver via USB
4. Click **[ESTABLISH LINK]** to connect

## ⚡ Requirements

- **Browser**: Chrome 89+ or Edge 89+ (Web Serial API support)
- **Connection**: USB cable to Axis Driver
- **Server**: Python 3 (or any HTTP server)

## 🔧 Features

- **Device Connection**: Serial communication with Axis Driver
- **System Configuration**: Firmware version, I2C, Ethernet settings
- **LED Control**: RGB color and animation control
- **Home Calibration**: Motor homing and calibration
- **Motor Control**: Position, velocity, and path control

## 🎨 Interface

The interface features a retro computing terminal aesthetic with:
- Green-on-black terminal color scheme
- CRT monitor effects (scan lines, flicker)
- Command-line style controls
- Real-time status monitoring
- Monospace terminal font

## 🔐 Security Notes

- The Web Serial API requires either HTTPS or localhost
- Make sure you trust the device you're connecting to
- Only connect genuine Axis Driver devices

## 🐛 Troubleshooting

### "Web Serial API not supported"
- Use Chrome 89+ or Edge 89+
- Ensure you're accessing via HTTP/HTTPS (not file://)

### "HTTPS Required"
- Use localhost instead of file:// protocol
- Serve the files via HTTP server (see instructions above)

### "No Port Selected" 
- Make sure Axis Driver is connected via USB
- Check that device drivers are installed
- Try a different USB cable/port

### "Port Busy"
- Close other applications that might be using the serial port
- Disconnect and reconnect the device
- Refresh the browser page

## 📁 Files

- `index.html` - Main interface
- `style.css` - Retro terminal styling
- `script.js` - Connection and control logic
- `AxisProtocol.js` - Protocol implementation
- `serve.py` - Python HTTP server
- `start_server.sh` - Quick start script

## 🎯 Usage

1. **Connect**: Click [ESTABLISH LINK] to connect via USB
2. **Configure**: Set device parameters (I2C, Ethernet, etc.)
3. **Control**: Use motor control commands
4. **Monitor**: View real-time status and feedback
5. **Disconnect**: Click [DISCONNECT] when done

The interface provides full control over the Axis Driver's configuration and motor control capabilities in an immersive retro computing environment.
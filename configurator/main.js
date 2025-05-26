
let port;
let reader;
let writer;

const connectBtn = document.getElementById('connectBtn');
const statusDiv = document.getElementById('status');
const settingsForm = document.getElementById('settingsForm');
const readBtn = document.getElementById('readBtn');
const writeBtn = document.getElementById('writeBtn');

connectBtn.addEventListener('click', async () => {
    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });
        statusDiv.textContent = 'Connected';
        settingsForm.style.display = 'block';
        writer = port.writable.getWriter();
        reader = port.readable.getReader();
    } catch (e) {
        statusDiv.textContent = 'Connection failed: ' + e;
    }
});

readBtn.addEventListener('click', async () => {
    await sendCommand('GET_SETTINGS');
    const buffer = await readBinaryResponse(256);
    if (buffer) {
        parseSettingsBuffer(buffer);
    } else {
        statusDiv.textContent = 'Failed to read settings.';
    }
});

writeBtn.addEventListener('click', async () => {
    const buffer = buildSettingsBuffer();
    await sendCommand('SET_SETTINGS', buffer);
    // Optionally, read response for confirmation
    const resp = await readTextResponse();
    statusDiv.textContent = resp.trim();
});

async function sendCommand(cmd, payload) {
    const encoder = new TextEncoder();
    let data = encoder.encode(cmd + '\n');
    await writer.write(data);
    if (payload) {
        await writer.write(payload);
    }
}

async function readBinaryResponse(length) {
    // Read exactly 'length' bytes from serial
    let received = new Uint8Array(length);
    let offset = 0;
    while (offset < length) {
        const { value, done } = await reader.read();
        if (done) break;
        if (value) {
            received.set(value.slice(0, length - offset), offset);
            offset += value.length;
        }
    }
    if (offset === length) return received;
    return null;
}

async function readTextResponse() {
    // Read until newline
    const decoder = new TextDecoder();
    let result = '';
    while (true) {
        const { value, done } = await reader.read();
        if (done) break;
        result += decoder.decode(value);
        if (result.includes('\n')) break;
    }
    return result;
}

function parseSettingsBuffer(buffer) {
    // EthernetSettingsStruct: uint16_t port, uint32_t ip_address
    const dv = new DataView(buffer.buffer);
    // Ethernet
    const port = dv.getUint16(0, true); // little-endian
    const ip = dv.getUint32(2, true);
    document.getElementById('eth_port').value = port;
    document.getElementById('eth_ip').value =
        ((ip >> 24) & 0xFF) + '.' + ((ip >> 16) & 0xFF) + '.' + ((ip >> 8) & 0xFF) + '.' + (ip & 0xFF);

    // I2CSettingsStruct: uint8_t address (offset 6)
    document.getElementById('i2c_address').value = buffer[6];

    // MotorSettingsStruct: uint8_t placeholder_settings (offset 7)
    document.getElementById('motor_max_speed').value = buffer[7];

    // HASettingsStruct: mode at offset 9 (enable=8, mode=9)
    document.getElementById('ha_mode').value = buffer[9];
}

function buildSettingsBuffer() {
    // Build a 256-byte buffer from form fields
    const buffer = new Uint8Array(256);
    const dv = new DataView(buffer.buffer);
    // Ethernet
    dv.setUint16(0, parseInt(document.getElementById('eth_port').value) || 0, true);
    const ipStr = document.getElementById('eth_ip').value.split('.');
    let ip = 0;
    if (ipStr.length === 4) {
        ip = (parseInt(ipStr[0]) << 24) | (parseInt(ipStr[1]) << 16) | (parseInt(ipStr[2]) << 8) | (parseInt(ipStr[3]));
    }
    dv.setUint32(2, ip >>> 0, true);
    // I2C
    buffer[6] = parseInt(document.getElementById('i2c_address').value) || 0;
    // Motor
    buffer[7] = parseInt(document.getElementById('motor_max_speed').value) || 0;
    // HA
    buffer[9] = parseInt(document.getElementById('ha_mode').value) || 0;
    // (Other fields left as zero)
    return buffer;
}

// Build a header: 1 byte message_type, 2 bytes body_size (little-endian)
function Header(message_type, body_size) {
    totalLength = 3; // 1 byte for message_type + 2 bytes for body_size
    let offset = 0;

    const buffer = new Uint8Array(3);
    
    const dv = new DataView(buffer.buffer);
    dv.setUint16(0, message_type); // Set message_type
    dv.setUint16(2, body_size, true); // Set body_size in little-endian format
    return buffer;

    return header;
}

//VelocityAndStepsMessage
function buildVelocityAndStepsMessage(message_type, velocity, steps, positionMode, footerBytes) {
    const totalLength = 9;
    const headerBytes = Header(message_type, bodySize);
    const totalLength = headerBytes.length + bodySize + footerBytes.length;
    const buffer = new Uint8Array(totalLength);
    let offset = 0;

    // Header
    buffer.set(headerBytes, offset);
    offset += headerBytes.length;

    // velocity (int32_t, little-endian)
    const dv = new DataView(buffer.buffer);
    dv.setUInt32(offset, velocity, true);
    offset += 4;

    // steps (int32_t, little-endian)
    dv.setInt32(offset, steps, true);
    offset += 4;

    // positionMode (uint8_t)
    buffer[offset] = positionMode;
    offset += 1;

    // Footer
    buffer.set(footerBytes, offset);

    return buffer;
}

// Parse a VelocityAndStepsMessage buffer
function  parseVelocityAndStepsMessage(buffer, footerLength) {
    const dv = new DataView(buffer.buffer, buffer.byteOffset, buffer.byteLength);
    let offset = 0;

    const messageType = dv.getUint16(offset, true);
    offset += 2;
    
    const bodySize = dv.getUint16(offset, true);
    offset += 2;

    const velocity = dv.getInt32(offset, true);
    offset += 4;

    const steps = dv.getInt32(offset, true);
    offset += 4;

    const positionMode = buffer[offset];


    return { velocity, steps, positionMode };
}
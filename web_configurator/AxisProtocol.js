// AxisProtocol.js - Complete message protocol implementation for Axis Driver
// Based on Axis Driver Protocol Documentation v1.0.0.0

// Message Type Constants
const MESSAGE_TYPES = {
  AckId: 0x0100,
  GetVersionId: 0x0101,
  SetI2CAddressId: 0x0102,
  GetI2CAddressId: 0x0103,
  SetEthernetAddressId: 0x0104,
  GetEthernetAddressId: 0x0105,
  SetEthernetPortId: 0x0106,
  GetEthernetPortId: 0x0107,
  GetMacAddressId: 0x0108,
  SaveConfigurationId: 0x0109,
  SetLedColorId: 0x0200,
  GetLedColorId: 0x0201,
  AddLedStepId: 0x0202,
  SetHomeDirectionId: 0x0300,
  GetHomeDirectionId: 0x0301,
  SetHomeThresholdId: 0x0302,
  GetHomeThresholdId: 0x0303,
  SetHomeSpeedId: 0x0304,
  GetHomeSpeedId: 0x0305,
  GetHomedStateId: 0x0306,
  HomeId: 0x0400,
  SetMotorStateId: 0x0307,
  GetMotorStateId: 0x0308,
  SetMotorBrakeId: 0x0309,
  GetMotorBrakeId: 0x030A,
  SetMaxSpeedId: 0x030B,
  GetMaxSpeedId: 0x030C,
  SetAccelerationId: 0x030D,
  GetAccelerationId: 0x030E,
  SetCurrentPositionId: 0x030F,
  GetCurrentPositionId: 0x0310,
  SetTargetPositionId: 0x0311,
  GetTargetPositionId: 0x0312,
  SetRelativeTargetPositionId: 0x0401,
  SetVelocityId: 0x0313,
  GetVelocityId: 0x0314,
  SetVelocityAndStepsId: 0x0402,
  StartPathId: 0x0403,
};

// Status Codes
const STATUS_CODES = {
  SUCCESS: 0x0,
  ERROR: 0x1,
  INVALID_COMMAND: 0x2,
};

// LED States
const LED_STATES = {
  OFF: 0x0,
  FLASH_ERROR: 0x1,
  ERROR: 0x2,
  BOOTUP: 0x3,
  RAINBOW: 0x4,
  SOLID: 0x5,
};

// Motor States
const MOTOR_STATES = {
  OFF: 0x0,
  POSITION: 0x1,
  VELOCITY: 0x2,
  VELOCITY_STEP: 0x3,
  IDLE_ON: 0x4,
  HOME: 0x5,
};

// Motor Brake Modes
const MOTOR_BRAKE = {
  NORMAL: 0x0,
  FREEWHEELING: 0x1,
  STRONG_BRAKING: 0x2,
  BRAKING: 0x3,
};

// Home Direction
const HOME_DIRECTION = {
  CLOCKWISE: 0x0,
  COUNTERCLOCKWISE: 0x1,
};

// Position Mode
const POSITION_MODE = {
  ABSOLUTE: 0x0,
  RELATIVE: 0x1,
};

// Calculate checksum (16-bit sum of header + body)
function calculateChecksum(data) {
  let sum = 0;
  for (let i = 0; i < data.length; i++) {
    sum += data[i];
  }
  return sum & 0xFFFF;
}

// Build message function
function buildMessage(type, body) {
  const header = new ArrayBuffer(4);
  const headerView = new DataView(header);
  headerView.setUint16(0, type, true); // little-endian
  headerView.setUint16(2, body.length, true);

  // Combine header + body
  const messageWithoutChecksum = new Uint8Array(header.byteLength + body.length);
  messageWithoutChecksum.set(new Uint8Array(header), 0);
  messageWithoutChecksum.set(body, header.byteLength);

  // Calculate checksum
  const checksum = calculateChecksum(messageWithoutChecksum);

  // Add footer
  const footer = new ArrayBuffer(2);
  const footerView = new DataView(footer);
  footerView.setUint16(0, checksum, true);

  // Final message
  const finalMessage = new Uint8Array(messageWithoutChecksum.length + footer.byteLength);
  finalMessage.set(messageWithoutChecksum, 0);
  finalMessage.set(new Uint8Array(footer), messageWithoutChecksum.length);

  return finalMessage;
}

// Message builders

function buildAckMessage(ackMessageType, status) {
  const body = new Uint8Array(3);
  const view = new DataView(body.buffer);
  view.setUint16(0, ackMessageType, true);
  view.setUint8(2, status);
  return buildMessage(MESSAGE_TYPES.AckId, body);
}

function buildGetVersion() {
  return buildMessage(MESSAGE_TYPES.GetVersionId, new Uint8Array(0));
}

function buildSetI2CAddress(address) {
  const body = new Uint8Array([address]);
  return buildMessage(MESSAGE_TYPES.SetI2CAddressId, body);
}

function buildGetI2CAddress() {
  return buildMessage(MESSAGE_TYPES.GetI2CAddressId, new Uint8Array(0));
}

function buildSetEthernetAddress(ipAddress) {
  const body = new ArrayBuffer(4);
  const view = new DataView(body);
  view.setUint32(0, ipAddress, true);
  return buildMessage(MESSAGE_TYPES.SetEthernetAddressId, new Uint8Array(body));
}

function buildGetEthernetAddress() {
  return buildMessage(MESSAGE_TYPES.GetEthernetAddressId, new Uint8Array(0));
}

function buildSetEthernetPort(port) {
  const body = new ArrayBuffer(4);
  const view = new DataView(body);
  view.setUint32(0, port, true);
  return buildMessage(MESSAGE_TYPES.SetEthernetPortId, new Uint8Array(body));
}

function buildGetEthernetPort() {
  return buildMessage(MESSAGE_TYPES.GetEthernetPortId, new Uint8Array(0));
}

function buildGetMacAddress() {
  return buildMessage(MESSAGE_TYPES.GetMacAddressId, new Uint8Array(0));
}

function buildSaveConfiguration(saveFlag = true) {
  const body = new Uint8Array([saveFlag ? 1 : 0]);
  return buildMessage(MESSAGE_TYPES.SaveConfigurationId, body);
}

function buildSetLedColor(r, g, b) {
  const body = new Uint8Array([r, g, b]);
  return buildMessage(MESSAGE_TYPES.SetLedColorId, body);
}

function buildGetLedColor() {
  return buildMessage(MESSAGE_TYPES.GetLedColorId, new Uint8Array(0));
}

function buildAddLedStep(timeMs, r, g, b) {
  const body = new ArrayBuffer(7);
  const view = new DataView(body);
  view.setUint32(0, timeMs, true);
  view.setUint8(4, r);
  view.setUint8(5, g);
  view.setUint8(6, b);
  return buildMessage(MESSAGE_TYPES.AddLedStepId, new Uint8Array(body));
}

function buildSetHomeDirection(direction) {
  const body = new Uint8Array([direction]);
  return buildMessage(MESSAGE_TYPES.SetHomeDirectionId, body);
}

function buildGetHomeDirection() {
  return buildMessage(MESSAGE_TYPES.GetHomeDirectionId, new Uint8Array(0));
}

function buildSetHomeThreshold(threshold) {
  const body = new ArrayBuffer(4);
  const view = new DataView(body);
  view.setUint32(0, threshold, true);
  return buildMessage(MESSAGE_TYPES.SetHomeThresholdId, new Uint8Array(body));
}

function buildGetHomeThreshold() {
  return buildMessage(MESSAGE_TYPES.GetHomeThresholdId, new Uint8Array(0));
}

function buildSetHomeSpeed(speed) {
  const body = new ArrayBuffer(4);
  const view = new DataView(body);
  view.setUint32(0, speed, true);
  return buildMessage(MESSAGE_TYPES.SetHomeSpeedId, new Uint8Array(body));
}

function buildGetHomeSpeed() {
  return buildMessage(MESSAGE_TYPES.GetHomeSpeedId, new Uint8Array(0));
}

function buildGetHomedState() {
  return buildMessage(MESSAGE_TYPES.GetHomedStateId, new Uint8Array(0));
}

function buildHome(homeCommand = 1) {
  const body = new ArrayBuffer(4);
  const view = new DataView(body);
  view.setUint32(0, homeCommand, true);
  return buildMessage(MESSAGE_TYPES.HomeId, new Uint8Array(body));
}

function buildSetMotorState(motorState) {
  const body = new Uint8Array([motorState]);
  return buildMessage(MESSAGE_TYPES.SetMotorStateId, body);
}

function buildGetMotorState() {
  return buildMessage(MESSAGE_TYPES.GetMotorStateId, new Uint8Array(0));
}

function buildSetMotorBrake(brakeMode) {
  const body = new Uint8Array([brakeMode]);
  return buildMessage(MESSAGE_TYPES.SetMotorBrakeId, body);
}

function buildGetMotorBrake() {
  return buildMessage(MESSAGE_TYPES.GetMotorBrakeId, new Uint8Array(0));
}

function buildSetMaxSpeed(maxSpeed) {
  const body = new ArrayBuffer(4);
  const view = new DataView(body);
  view.setUint32(0, maxSpeed, true);
  return buildMessage(MESSAGE_TYPES.SetMaxSpeedId, new Uint8Array(body));
}

function buildGetMaxSpeed() {
  return buildMessage(MESSAGE_TYPES.GetMaxSpeedId, new Uint8Array(0));
}

function buildSetAcceleration(acceleration) {
  const body = new ArrayBuffer(4);
  const view = new DataView(body);
  view.setUint32(0, acceleration, true);
  return buildMessage(MESSAGE_TYPES.SetAccelerationId, new Uint8Array(body));
}

function buildGetAcceleration() {
  return buildMessage(MESSAGE_TYPES.GetAccelerationId, new Uint8Array(0));
}

function buildSetCurrentPosition(position) {
  const body = new ArrayBuffer(8);
  const view = new DataView(body);
  view.setFloat64(0, position, true);
  return buildMessage(MESSAGE_TYPES.SetCurrentPositionId, new Uint8Array(body));
}

function buildGetCurrentPosition() {
  return buildMessage(MESSAGE_TYPES.GetCurrentPositionId, new Uint8Array(0));
}

function buildSetTargetPosition(position) {
  const body = new ArrayBuffer(8);
  const view = new DataView(body);
  view.setFloat64(0, position, true);
  return buildMessage(MESSAGE_TYPES.SetTargetPositionId, new Uint8Array(body));
}

function buildGetTargetPosition() {
  return buildMessage(MESSAGE_TYPES.GetTargetPositionId, new Uint8Array(0));
}

function buildSetRelativeTargetPosition(relativePosition) {
  const body = new ArrayBuffer(8);
  const view = new DataView(body);
  view.setFloat64(0, relativePosition, true);
  return buildMessage(MESSAGE_TYPES.SetRelativeTargetPositionId, new Uint8Array(body));
}

function buildSetVelocity(velocity) {
  const body = new ArrayBuffer(8);
  const view = new DataView(body);
  view.setFloat64(0, velocity, true);
  return buildMessage(MESSAGE_TYPES.SetVelocityId, new Uint8Array(body));
}

function buildGetVelocity() {
  return buildMessage(MESSAGE_TYPES.GetVelocityId, new Uint8Array(0));
}

function buildSetVelocityAndSteps(velocity, steps, positionMode) {
  const body = new ArrayBuffer(9);
  const view = new DataView(body);
  view.setInt32(0, velocity, true);
  view.setInt32(4, steps, true);
  view.setUint8(8, positionMode);
  return buildMessage(MESSAGE_TYPES.SetVelocityAndStepsId, new Uint8Array(body));
}

function buildStartPath(pathId) {
  const body = new Uint8Array([pathId]);
  return buildMessage(MESSAGE_TYPES.StartPathId, body);
}

// Message parsers

function parseAck(data) {
  if (data.length !== 9) throw new Error('Invalid ACK message length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const ackType = view.getUint16(0, true);
  const status = view.getUint8(2);
  return { ackType, status };
}

function parseGetVersion(data) {
  if (data.length !== 10) throw new Error('Invalid Get Version response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const version = view.getUint32(0, true);
  return { version };
}

function parseGetI2CAddress(data) {
  if (data.length !== 7) throw new Error('Invalid Get I2C Address response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const address = view.getUint8(0);
  return { address };
}

function parseGetEthernetAddress(data) {
  if (data.length !== 10) throw new Error('Invalid Get Ethernet Address response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const ipAddress = view.getUint32(0, true);
  return { ipAddress };
}

function parseGetEthernetPort(data) {
  if (data.length !== 10) throw new Error('Invalid Get Ethernet Port response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const port = view.getUint32(0, true);
  return { port };
}

function parseGetMacAddress(data) {
  if (data.length !== 12) throw new Error('Invalid Get MAC Address response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const mac = [];
  for (let i = 0; i < 6; i++) {
    mac.push(view.getUint8(i));
  }
  return { mac };
}

function parseGetLedColor(data) {
  if (data.length !== 9) throw new Error('Invalid Get LED Color response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const r = view.getUint8(0);
  const g = view.getUint8(1);
  const b = view.getUint8(2);
  return { r, g, b };
}

function parseGetHomeDirection(data) {
  if (data.length !== 7) throw new Error('Invalid Get Home Direction response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const direction = view.getUint8(0);
  return { direction };
}

function parseGetHomeThreshold(data) {
  if (data.length !== 10) throw new Error('Invalid Get Home Threshold response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const threshold = view.getUint32(0, true);
  return { threshold };
}

function parseGetHomeSpeed(data) {
  if (data.length !== 10) throw new Error('Invalid Get Home Speed response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const speed = view.getUint32(0, true);
  return { speed };
}

function parseGetHomedState(data) {
  if (data.length !== 7) throw new Error('Invalid Get Homed State response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const homed = view.getUint8(0) !== 0;
  return { homed };
}

function parseGetMotorState(data) {
  if (data.length !== 7) throw new Error('Invalid Get Motor State response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const motorState = view.getUint8(0);
  return { motorState };
}

function parseGetMotorBrake(data) {
  if (data.length !== 7) throw new Error('Invalid Get Motor Brake response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const brakeMode = view.getUint8(0);
  return { brakeMode };
}

function parseGetMaxSpeed(data) {
  if (data.length !== 10) throw new Error('Invalid Get Max Speed response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const maxSpeed = view.getUint32(0, true);
  return { maxSpeed };
}

function parseGetAcceleration(data) {
  if (data.length !== 10) throw new Error('Invalid Get Acceleration response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const acceleration = view.getUint32(0, true);
  return { acceleration };
}

function parseGetCurrentPosition(data) {
  if (data.length !== 14) throw new Error('Invalid Get Current Position response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const position = view.getFloat64(0, true);
  return { position };
}

function parseGetTargetPosition(data) {
  if (data.length !== 14) throw new Error('Invalid Get Target Position response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const position = view.getFloat64(0, true);
  return { position };
}

function parseGetVelocity(data) {
  if (data.length !== 14) throw new Error('Invalid Get Velocity response length');
  const view = new DataView(data.buffer, data.byteOffset + 4);
  const velocity = view.getFloat64(0, true);
  return { velocity };
}

// Utility functions

function parseMessageHeader(data) {
  if (data.length < 4) throw new Error('Data too short for header');
  const view = new DataView(data.buffer, data.byteOffset);
  const messageType = view.getUint16(0, true);
  const bodySize = view.getUint16(2, true);
  return { messageType, bodySize };
}

function verifyChecksum(data) {
  if (data.length < 6) return false;
  const receivedChecksum = new DataView(data.buffer, data.byteOffset + data.length - 2).getUint16(0, true);
  const calculatedChecksum = calculateChecksum(data.slice(0, -2));
  return receivedChecksum === calculatedChecksum;
}

function formatMessageBytes(data, bytesPerLine = 16) {
  let result = '';
  for (let i = 0; i < data.length; i += bytesPerLine) {
    const chunk = data.slice(i, i + bytesPerLine);
    const hexPart = Array.from(chunk).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' ');
    const asciiPart = Array.from(chunk).map(b => (b >= 32 && b <= 126) ? String.fromCharCode(b) : '.').join('');
    result += `${i.toString(16).padStart(4, '0').toUpperCase()}: ${hexPart.padEnd(bytesPerLine * 3 - 1)} ${asciiPart}\n`;
  }
  return result;
}

// Export functions (for Node.js or module systems)
if (typeof module !== 'undefined' && module.exports) {
  module.exports = {
    MESSAGE_TYPES,
    STATUS_CODES,
    LED_STATES,
    MOTOR_STATES,
    MOTOR_BRAKE,
    HOME_DIRECTION,
    POSITION_MODE,
    buildMessage,
    // Builders
    buildAckMessage,
    buildGetVersion,
    buildSetI2CAddress,
    buildGetI2CAddress,
    buildSetEthernetAddress,
    buildGetEthernetAddress,
    buildSetEthernetPort,
    buildGetEthernetPort,
    buildGetMacAddress,
    buildSaveConfiguration,
    buildSetLedColor,
    buildGetLedColor,
    buildAddLedStep,
    buildSetHomeDirection,
    buildGetHomeDirection,
    buildSetHomeThreshold,
    buildGetHomeThreshold,
    buildSetHomeSpeed,
    buildGetHomeSpeed,
    buildGetHomedState,
    buildHome,
    buildSetMotorState,
    buildGetMotorState,
    buildSetMotorBrake,
    buildGetMotorBrake,
    buildSetMaxSpeed,
    buildGetMaxSpeed,
    buildSetAcceleration,
    buildGetAcceleration,
    buildSetCurrentPosition,
    buildGetCurrentPosition,
    buildSetTargetPosition,
    buildGetTargetPosition,
    buildSetRelativeTargetPosition,
    buildSetVelocity,
    buildGetVelocity,
    buildSetVelocityAndSteps,
    buildStartPath,
    // Parsers
    parseAck,
    parseGetVersion,
    parseGetI2CAddress,
    parseGetEthernetAddress,
    parseGetEthernetPort,
    parseGetMacAddress,
    parseGetLedColor,
    parseGetHomeDirection,
    parseGetHomeThreshold,
    parseGetHomeSpeed,
    parseGetHomedState,
    parseGetMotorState,
    parseGetMotorBrake,
    parseGetMaxSpeed,
    parseGetAcceleration,
    parseGetCurrentPosition,
    parseGetTargetPosition,
    parseGetVelocity,
    // Utilities
    parseMessageHeader,
    verifyChecksum,
    formatMessageBytes,
  };
}


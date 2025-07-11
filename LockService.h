/*
 * LockService.h
 * 
 * HomeKit Lock Service implementation for ESP32
 * Provides lock control and status monitoring capabilities
 * 
 * This service implements:
 * - LockCurrentState (read-only)
 * - LockTargetState (read/write)
 * - Optional: StatusLowBattery, StatusFault, StatusJammed
 */

#ifndef LOCK_SERVICE_H
#define LOCK_SERVICE_H

#include "HomeSpan.h"

// External function declarations
extern void updateLockState(int lockIndex, bool isLocked);
extern bool lockStates[];

// Lock Service Class
class LockService : public Service::LockManagement {
  
private:
  int lockIndex;
  SpanCharacteristic *lockCurrentState;
  SpanCharacteristic *lockTargetState;
  SpanCharacteristic *statusLowBattery;
  SpanCharacteristic *statusFault;
  SpanCharacteristic *statusJammed;
  
  // Timing variables for realistic lock operation simulation
  unsigned long operationStartTime;
  bool operationInProgress;
  int targetState;
  
  // Lock state constants (HomeKit specification)
  enum LockState {
    UNSECURED = 0,  // Unlocked
    SECURED = 1,    // Locked
    JAMMED = 2,     // Jammed
    UNKNOWN = 3     // Unknown state
  };
  
  // Status flags
  bool lowBattery;
  bool fault;
  bool jammed;
  
public:
  // Constructor
  LockService(int index) : Service::LockManagement() {
    lockIndex = index;
    operationInProgress = false;
    targetState = UNSECURED;
    lowBattery = false;
    fault = false;
    jammed = false;
    
    // Initialize lock characteristics
    lockCurrentState = new Characteristic::LockCurrentState(UNSECURED);
    lockTargetState = new Characteristic::LockTargetState(UNSECURED);
    
    // Optional status characteristics
    statusLowBattery = new Characteristic::StatusLowBattery(false);
    statusFault = new Characteristic::StatusFault(false);
    statusJammed = new Characteristic::StatusJammed(false);
    
    Serial.print("Lock Service ");
    Serial.print(lockIndex + 1);
    Serial.println(" initialized");
    
    // Set range validation for characteristics
    lockCurrentState->setRange(0, 3, 1);  // 0-3: Unsecured, Secured, Jammed, Unknown
    lockTargetState->setRange(0, 1, 1);   // 0-1: Unsecured, Secured
  }
  
  // HomeKit update method - called when characteristics change
  boolean update() {
    
    // Check if target state has changed
    if(lockTargetState->updated()) {
      int newTargetState = lockTargetState->getNewVal();
      
      Serial.print("Lock ");
      Serial.print(lockIndex + 1);
      Serial.print(" target state changed to: ");
      Serial.println(newTargetState == SECURED ? "LOCKED" : "UNLOCKED");
      
      // Start lock operation
      targetState = newTargetState;
      operationStartTime = millis();
      operationInProgress = true;
      
      // Update current state to unknown during operation
      lockCurrentState->setVal(UNKNOWN);
      
      // Update global state
      updateLockState(lockIndex, (targetState == SECURED));
    }
    
    return true;
  }
  
  // Loop method - called continuously to handle ongoing operations
  void loop() {
    
    // Handle lock operation timing (simulate realistic lock operation)
    if(operationInProgress) {
      unsigned long elapsed = millis() - operationStartTime;
      
      // Simulate lock operation time (1-3 seconds)
      unsigned long operationTime = 1500 + (lockIndex * 300); // Vary by lock
      
      if(elapsed >= operationTime) {
        // Operation complete
        operationInProgress = false;
        
        // Random chance of jam or fault (for demonstration)
        if(random(100) < 2) { // 2% chance of jam
          jammed = true;
          lockCurrentState->setVal(JAMMED);
          statusJammed->setVal(true);
          Serial.print("Lock ");
          Serial.print(lockIndex + 1);
          Serial.println(" JAMMED!");
        }
        else if(random(100) < 1) { // 1% chance of fault
          fault = true;
          statusFault->setVal(true);
          lockCurrentState->setVal(UNKNOWN);
          Serial.print("Lock ");
          Serial.print(lockIndex + 1);
          Serial.println(" FAULT detected!");
        }
        else {
          // Normal operation
          jammed = false;
          fault = false;
          statusJammed->setVal(false);
          statusFault->setVal(false);
          lockCurrentState->setVal(targetState);
          
          Serial.print("Lock ");
          Serial.print(lockIndex + 1);
          Serial.print(" operation complete: ");
          Serial.println(targetState == SECURED ? "LOCKED" : "UNLOCKED");
        }
      }
    }
    
    // Simulate battery status (for demonstration)
    static unsigned long lastBatteryCheck = 0;
    if(millis() - lastBatteryCheck > 30000) { // Check every 30 seconds
      lastBatteryCheck = millis();
      
      // Random low battery simulation (5% chance)
      bool newLowBattery = (random(100) < 5);
      if(newLowBattery != lowBattery) {
        lowBattery = newLowBattery;
        statusLowBattery->setVal(lowBattery);
        if(lowBattery) {
          Serial.print("Lock ");
          Serial.print(lockIndex + 1);
          Serial.println(" - LOW BATTERY WARNING!");
        }
      }
    }
    
    // Sync with global state
    syncWithGlobalState();
  }
  
  // Method to manually set lock state (for external control)
  void setLockState(bool locked) {
    if(!operationInProgress) {
      int newState = locked ? SECURED : UNSECURED;
      lockCurrentState->setVal(newState);
      lockTargetState->setVal(newState);
      targetState = newState;
      
      // Clear any error states
      jammed = false;
      fault = false;
      statusJammed->setVal(false);
      statusFault->setVal(false);
    }
  }
  
  // Get current lock state
  bool isLocked() {
    return (lockCurrentState->getVal() == SECURED);
  }
  
  // Get lock status information
  String getStatusString() {
    String status = "Lock " + String(lockIndex + 1) + ": ";
    
    switch(lockCurrentState->getVal()) {
      case UNSECURED:
        status += "UNLOCKED";
        break;
      case SECURED:
        status += "LOCKED";
        break;
      case JAMMED:
        status += "JAMMED";
        break;
      case UNKNOWN:
        status += "UNKNOWN";
        break;
    }
    
    if(operationInProgress) {
      status += " (Operation in progress...)";
    }
    
    if(lowBattery) status += " [LOW BATTERY]";
    if(fault) status += " [FAULT]";
    if(jammed) status += " [JAMMED]";
    
    return status;
  }
  
  // Clear error states
  void clearErrors() {
    jammed = false;
    fault = false;
    lowBattery = false;
    statusJammed->setVal(false);
    statusFault->setVal(false);
    statusLowBattery->setVal(false);
    
    if(lockCurrentState->getVal() == JAMMED || lockCurrentState->getVal() == UNKNOWN) {
      lockCurrentState->setVal(targetState);
    }
    
    Serial.print("Lock ");
    Serial.print(lockIndex + 1);
    Serial.println(" errors cleared");
  }
  
private:
  // Sync with global lock states array
  void syncWithGlobalState() {
    if(!operationInProgress && !jammed && !fault) {
      bool globalState = lockStates[lockIndex];
      bool currentState = (lockCurrentState->getVal() == SECURED);
      
      if(globalState != currentState) {
        setLockState(globalState);
      }
    }
  }
};

#endif // LOCK_SERVICE_H
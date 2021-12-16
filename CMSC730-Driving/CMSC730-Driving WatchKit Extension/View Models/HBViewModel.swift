//
//  HBViewModel.swift
//  CMSC730-Driving WatchKit Extension
//
//  Created by William Ingold on 12/12/21.
//

import Foundation
import CoreBluetooth
import WatchKit

class HBViewModel : NSObject, ObservableObject, Identifiable {
    var id = UUID()
    
    @Published var output = "Disconnected"
    @Published var connected = false
    
    private var centralQueue: DispatchQueue?
    
    private let serviceUUID = CBUUID(string: "FFE0")
    
    private let charUUID = CBUUID(string: "FFE1")
    private var char: CBCharacteristic?
    
    private var centralManager: CBCentralManager?
    private var connectedPeripheral: CBPeripheral?
    
    func send(_ heartRate: Data) {
        guard let peripheral = connectedPeripheral,
              let inputChar = char else {
                  output = "Connection error"
                  return
              }
        
        output = "Sending..."
        peripheral.writeValue(heartRate, for: inputChar, type: .withoutResponse) // Or withresponse?
    }
    
    func connectDriver() {
        output = "Connecting..."
        centralQueue = DispatchQueue(label: "test.discovery")
        centralManager = CBCentralManager(delegate: self, queue: centralQueue)
    }
    
    func disconnectDriver() {
        guard let manager = centralManager,
              let peripheral = connectedPeripheral else { return }
                
        manager.cancelPeripheralConnection(peripheral)
    }
}

extension HBViewModel: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        print("Central Manager state changed: \(central.state)")
        if central.state == .poweredOn {
            central.scanForPeripherals(withServices: [serviceUUID], options: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        print("Discovered \(peripheral.name ?? "UNKNOWN")")
        central.stopScan()
        
        connectedPeripheral = peripheral
        central.connect(peripheral, options: nil)
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected to \(peripheral.name ?? "UNKNOWN")")
        peripheral.delegate = self
        peripheral.discoverServices(nil)
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Disconnected from \(peripheral.name ?? "UNKNOWN")")
        
        centralManager = nil
        
        DispatchQueue.main.async {
            self.connected = false
            self.output = "Disconnected"
        }
    }
}

extension HBViewModel : CBPeripheralDelegate {
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        print("Discovered services for \(peripheral.name ?? "UNKNOWN")")
                
        guard let services = peripheral.services else {
            return
        }
        
        for service in services {
            peripheral.discoverCharacteristics(nil, for: service)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        print("Discovered characteristics for \(peripheral.name ?? "UNKNOWN")")
        
        guard let characteristics = service.characteristics else {
            return
        }
        
        for ch in characteristics {
            switch ch.uuid {
            case charUUID:
                char = ch
                peripheral.setNotifyValue(true, for: ch)
            default:
                print("Default char \(ch)")
                break
            }
        }
        
        DispatchQueue.main.async {
            self.connected = true
            self.output = "Connected."
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        print("Notification state changed to \(characteristic.isNotifying)")
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        print("Characteristic updated: \(characteristic.uuid)")
        
        if characteristic.uuid == charUUID, let data = characteristic.value {
            let bytes:[UInt8] = data.map {$0}
            
            if let answer = bytes.first {
                DispatchQueue.main.async {
                    self.output = "WARNING!"
                    print("Answer: \(answer)")
                }
                
                WKInterfaceDevice.current().play(WKHapticType.notification)
            }
        }
    }
}

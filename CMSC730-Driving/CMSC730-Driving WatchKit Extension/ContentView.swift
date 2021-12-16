/**
 * Credit to Luciano Amoroso:
 * https://github.com/amorosoluciano/SwiftFun/blob/master/ContentView.swift
 *
 * For base work for heart rate measurement
 *
 * Credit to Rob Kerr for Bluetooth Comms and some design elements
 * https://github.com/robkerr/BlogProjects/tree/main/BLECalculator
 */

import SwiftUI
import HealthKit
import CoreBluetooth

struct ContentView: View {
    private var healthStore = HKHealthStore()
    let heartRateQuantity = HKUnit(from: "count/min")
    
    
    @StateObject var viewModel = HBViewModel()
    // private var hbTimer: Timer?
    
    @State private var value = 0
    
    var body: some View {
        VStack{
            HStack{
                Text("\(value)")
                    .fontWeight(.regular)
                    .font(.system(size: 70))
                
                Text("BPM")
                    .font(.headline)
                    .fontWeight(.bold)
                    .foregroundColor(Color.red)
                    .padding(.bottom, 28.0)
                
                Spacer()

            }
            
            HStack{
                Text("\(viewModel.output)")
            }
            
            HStack{
                ConnectButtonView(viewModel: viewModel)
            }
            


        }
        .padding()
        .onAppear(perform: start)
    }

    func start() {
        authorizeHealthKit()
        startHeartRateQuery(quantityTypeIdentifier: .heartRate)
        viewModel.connectDriver()
        /*
        Timer.scheduledTimer(withTimeInterval: 5.0, repeats: true, block: { timer in
            print("Sending \(value)")
            let data = withUnsafeBytes(of: value) { Data($0) }
            viewModel.send(data)
        })
         */
    }
    
    func authorizeHealthKit() {
        let healthKitTypes: Set = [
        HKObjectType.quantityType(forIdentifier: HKQuantityTypeIdentifier.heartRate)!]

        healthStore.requestAuthorization(toShare: healthKitTypes, read: healthKitTypes) { _, _ in }
    }
    
    private func startHeartRateQuery(quantityTypeIdentifier: HKQuantityTypeIdentifier) {
        
        // 1
        let devicePredicate = HKQuery.predicateForObjects(from: [HKDevice.local()])
        // 2
        let updateHandler: (HKAnchoredObjectQuery, [HKSample]?, [HKDeletedObject]?, HKQueryAnchor?, Error?) -> Void = {
            query, samples, deletedObjects, queryAnchor, error in
            
            // 3
        guard let samples = samples as? [HKQuantitySample] else {
            return
        }
            
        self.process(samples, type: quantityTypeIdentifier)

        }
        
        // 4
        let query = HKAnchoredObjectQuery(type: HKObjectType.quantityType(forIdentifier: quantityTypeIdentifier)!, predicate: devicePredicate, anchor: nil, limit: HKObjectQueryNoLimit, resultsHandler: updateHandler)
        
        query.updateHandler = updateHandler
        
        // 5
        
        healthStore.execute(query)
    }
    
    private func process(_ samples: [HKQuantitySample], type: HKQuantityTypeIdentifier) {
        var lastHeartRate = 0.0
        
        for sample in samples {
            if type == .heartRate {
                lastHeartRate = sample.quantity.doubleValue(for: heartRateQuantity)
            }
            
            self.value = Int(lastHeartRate)

            if (viewModel.connected) {
                let data = withUnsafeBytes(of: self.value) { Data($0) }
                viewModel.send(data)
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}

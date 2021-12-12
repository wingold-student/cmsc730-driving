//
//  CMSC730_DrivingApp.swift
//  CMSC730-Driving WatchKit Extension
//
//  Created by William Ingold on 12/11/21.
//

import SwiftUI

@main
struct CMSC730_DrivingApp: App {
    @Environment(\.scenePhase) var scenePhase

    
    @SceneBuilder var body: some Scene {
        WindowGroup {
            NavigationView {
                ContentView()
            }
        }
        .onChange(of: scenePhase, perform: { newScenePhase in
            switch newScenePhase {
            case .active:
                print("Active")
            case .inactive:
                print("Inactive")
            case .background:
                print("Background")
            @unknown default:
                print("Unexpected scene phase")
            }
        })

        WKNotificationScene(controller: NotificationController.self, category: "myCategory")
    }
}

//
//  ConnectButtonView.swift
//  BLECalculator
//
//  Created by Rob Kerr on 8/7/21.
//

import SwiftUI

struct ConnectButtonView: View {
    @ObservedObject var viewModel: HBViewModel
    
    var body: some View {
        HStack {
            if viewModel.connected {
                Button(action: {
                    viewModel.disconnectDriver()
                }, label: {
                    Text("Disconnect")
                        .padding()
                        .frame(maxWidth: .infinity)
                        .padding()
                }).buttonStyle(ConnectButton())
            } else {
                Button(action: {
                    viewModel.connectDriver()
                }, label: {
                    Text("Connect")
                        .padding()
                        .frame(maxWidth: .infinity)
                        .padding()
                }).buttonStyle(ConnectButton())
            }
        }
        .frame(width: 220)
        .padding(.top)
    }
}

struct ConnectButtonView_Previews: PreviewProvider {
    static var viewModel = HBViewModel()
    static var previews: some View {
        ConnectButtonView(viewModel: viewModel)
    }
}

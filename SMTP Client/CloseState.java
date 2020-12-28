/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtpclientprogram;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author lelmil
 */
public class CloseState implements State {

    @Override
    public void doAction(Context context, String instruction) {
        //try to connect (setting in, pr
        //if connected - go Begin State
        //else - keep this state 
        try {
            String mailServer = "webmail.buet.ac.bd";
            InetAddress mailHost = InetAddress.getByName(mailServer);
            InetAddress localHost = InetAddress.getLocalHost();
            context.smtpSocket = new Socket();
            context.smtpSocket.connect(new InetSocketAddress(mailHost, 25), 20000);
            context.smtpSocket.setSoTimeout(20000);
            context.in = new BufferedReader(new InputStreamReader(context.smtpSocket.getInputStream()));
            context.pr = new PrintWriter(context.smtpSocket.getOutputStream(), true);
            String initialID = context.in.readLine();
            System.out.println(initialID);
            
            if (initialID.startsWith("2")) {
                context.setState(new BeginState());
            }
        } catch (SocketTimeoutException ex) {
            //Logger.getLogger(CloseState.class.getName()).log(Level.SEVERE, null, ex);
            System.out.println("Time-out. Program aborted. Try again Later.");
            context.closeConnection();
        } catch (IOException ex){
            Logger.getLogger(CloseState.class.getName()).log(Level.SEVERE, null, ex);
            context.closeConnection();
        }

        //context.doJob();
    }
    
    @Override
    public void printState(){
        System.out.println("Closed State.");
    }

}

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtpclientprogram;

import java.io.IOException;
import java.net.SocketTimeoutException;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author lelmil
 */
public class WaitState implements State {

    @Override
    public void doAction(Context context, String instruction) {
        //System.out.println("Wait state.");
        //Scanner sc = new Scanner(System.in);
        //String cmd = sc.nextLine();
        String cmd = instruction;
        System.out.println(cmd);
        try {
            String reply = context.WriteToServer(cmd);
            System.out.println(reply);
            if( cmd.toUpperCase().startsWith("MAIL FROM") && reply.startsWith("2")){
                context.setState(new EnvelopeState());
            }
        } catch (SocketTimeoutException ex){
            System.out.println("Time-out. Program aborted. Try again Later.");
            context.closeConnection();
            context.setState(new CloseState());
        } catch (IOException ex) {
            Logger.getLogger(BeginState.class.getName()).log(Level.SEVERE, null, ex);
            context.closeConnection();
            context.setState(new CloseState());
        }
    }
    
    
    @Override
    public void printState(){
        System.out.println("Wait State.");
    }
    
}

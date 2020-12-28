/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtpclientprogram;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.SocketTimeoutException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author lelmil
 */
public class WritingState implements State{

    @Override
    public void doAction(Context context, String instruction) {
        System.out.println("Writing State " +instruction);
        try{
            File msg_body_file = new File(instruction);
            FileInputStream fis_msg = new FileInputStream(msg_body_file); 
            BufferedReader  br_msg = new BufferedReader(new InputStreamReader(fis_msg));
            String msg = "", tmp;
            tmp = br_msg.readLine();
            while(tmp!=null){
                msg = msg + tmp + "\n";
                if(tmp.equals(".")){
                    break;
                }
                tmp = br_msg.readLine();
            }
            //System.out.println(msg);
            String reply = context.WriteToServer(msg);
            System.out.println(reply);
            context.closeConnection();
            fis_msg.close();
            br_msg.close();
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
        System.out.println("Writing State.");
    }
    
}

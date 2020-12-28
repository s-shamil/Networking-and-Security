/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtpclientprogram;
import java.io.*;
import java.net.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Stream;


/**
 *
 * @author lelmil
 */
public class Context {
    BufferedReader in;
    PrintWriter pr;
    Socket smtpSocket;
    State state;
    
    boolean abortFlag;
    
    public Context(){
        in = null;
        pr = null;
        state = new CloseState();
        abortFlag = false;
    }
    
    void doJob(String instruction){
        state.doAction(this, instruction);
    }
    
    void closeConnection(){
        abortFlag = true;
        this.state = new CloseState();
        try {
            if(this.in!=null) this.in.close();
            if(this.pr!=null) this.pr.close();
            if(this.smtpSocket!=null) this.smtpSocket.close();
        } catch (IOException ex) {
            Logger.getLogger(Context.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    String WriteToServer(String msg) throws SocketTimeoutException, IOException{
        pr.println(msg);
        pr.flush();
        msg = in.readLine();
        return msg;
    }
    
    State getState(){
        return this.state;
    }
    
    void setState(State state){
        this.state = state;
    }
    
}

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtpclientprogram;

/**
 *
 * @author lelmil
 */
public interface State {
    public void doAction(Context context, String instruction);
    public void printState();
}

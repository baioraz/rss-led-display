/*******************************************************************************
 * Copyright (c) 2000, 2006 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package lt.avr.movingmessage;
/* 
 * example snippet: ExpandBar example
 *
 * For a list of all SWT example snippets see
 * http://www.eclipse.org/swt/snippets/
 * 
 * @since 3.2
 */

import lt.avr.movingmessage.cable.MessageSender;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

public class MovingMessageApp {

public static void main (String [] args) {
	System.out.println(System.currentTimeMillis());
	
	Display display = new Display ();
	Shell shell = new Shell (display);
	GridLayout gridLayout = new GridLayout ();
	gridLayout.numColumns = 4;

	shell.setLayout(gridLayout);
	shell.setText("Moving Message");
	
	GridData gridData = new GridData();
	gridData.horizontalSpan = 2;
	Label logo = new Label(shell, SWT.NONE);
	logo.setText("LOGO");
	logo.setLayoutData(gridData);
	
	gridData = new GridData();
	gridData.horizontalSpan = 2;
	Label version = new Label(shell, SWT.NONE);
	version.setText("Version 'Alpha'");
	version.setLayoutData(gridData);
	
	gridData = new GridData();
	Label statusLabel = new Label(shell, SWT.NONE);
	statusLabel.setText("Status: ");
	statusLabel.setLayoutData(gridData);	
	
	gridData = new GridData();
	Label status = new Label(shell, SWT.NONE);
	status.setText("Not Connected");
	status.setLayoutData(gridData);		
	
	gridData = new GridData();
	Label speedLabel = new Label(shell, SWT.NONE);
	speedLabel.setText("Speed: ");
	speedLabel.setLayoutData(gridData);	
	
	gridData = new GridData();
	CCombo speedCombo = new CCombo(shell, SWT.NONE);
	//scpeedCombo.setText("Not Connected");
	speedCombo.add("1");
	speedCombo.add("2");
	speedCombo.add("3");
	speedCombo.add("4");
	speedCombo.add("5");
	speedCombo.setLayoutData(gridData);	
	
	gridData = new GridData(GridData.HORIZONTAL_ALIGN_FILL | GridData.VERTICAL_ALIGN_FILL);
	gridData.horizontalSpan = 4;
	gridData.grabExcessVerticalSpace = true;
	final Text messageText = new Text(shell, SWT.BORDER | SWT.MULTI | SWT.WRAP);
	messageText.setText("Not Connected");
	messageText.setLayoutData(gridData);	
	
	gridData = new GridData();
	gridData.horizontalSpan = 3;
	ProgressBar progress = new ProgressBar(shell, SWT.NONE);
	progress.setLayoutData(gridData);	
	
	gridData = new GridData();
	final Button button = new Button(shell, SWT.NONE);
	button.setText("Write");
	button.setLayoutData(gridData);		
	
	button.addSelectionListener(new SelectionListener() {

			public void widgetSelected(SelectionEvent event) {
				MessageSender.sendMessage(messageText.getText());
			}

			public void widgetDefaultSelected(SelectionEvent event) {
				button.setText("No worries!");
			}
		});

	
	Label logo3 = new Label(shell, SWT.NONE);
	logo3.setText("LOGO");
	Label logo4 = new Label(shell, SWT.NONE);
	logo4.setText("LOGO");
	Label logo5 = new Label(shell, SWT.NONE);
	logo5.setText("LOGO");
	Label logo6 = new Label(shell, SWT.NONE);
	logo6.setText("LOGO");
	
	
	
//	ExpandBar bar = new ExpandBar (shell, SWT.V_SCROLL);
//	Image image = display.getSystemImage(SWT.ICON_QUESTION);
//	
//	// First item
//	composite = new Composite (bar, SWT.NONE);
//	layout = new GridLayout (1, true);
//	layout.marginLeft = layout.marginTop = layout.marginRight = layout.marginBottom = 0;
//	layout.verticalSpacing = 0;
//	composite.setLayout(layout);
//	
//
//
//	
//	final Button button = new Button (composite, SWT.PUSH);
//	button.setText("SWT.PUSH");
//
//
//	
//    button.addSelectionListener(new SelectionListener() {
//
//        public void widgetSelected(SelectionEvent event) {
//          button.setText("No vvvv worries!");
//        }
//
//        public void widgetDefaultSelected(SelectionEvent event) {
//        	button.setText("No worries!");
//        }
//      });
//	
//	
//	ExpandItem item0 = new ExpandItem (bar, SWT.NONE, 0);
//	item0.setText("General");
//	item0.setHeight(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT).y);
//	item0.setControl(composite);
//	//item0.setImage(image);
//	
//	
//	button.addKeyListener(new KeyListener() {
//	      String selectedItem = "";
//
//	      public void keyPressed(KeyEvent e) {
//	    	//button.setText("");
//	      }
//
//	      public void keyReleased(KeyEvent e) {
//	        button.setText("sdf");
//	      }
//	    });
//    
//	
//	// Third item
//	composite = new Composite (bar, SWT.NONE);
//	layout = new GridLayout (2, true);
//	layout.marginLeft = layout.marginTop = layout.marginRight = layout.marginBottom = 10;
//	layout.verticalSpacing = 10;
//	composite.setLayout(layout);
//	Label label = new Label (composite, SWT.NONE);
//	label.setText("Scale");	
//	new Scale (composite, SWT.NONE);
//	label = new Label (composite, SWT.NONE);
//	label.setText("Spinner");	
//	new Spinner (composite, SWT.BORDER);
//	label = new Label (composite, SWT.NONE);
//	label.setText("Slider");	
//	new Slider (composite, SWT.NONE);
//	ExpandItem item2 = new ExpandItem (bar, SWT.NONE, 1);
//	item2.setText("What is your favorite range widget");
//	item2.setHeight(composite.computeSize(SWT.DEFAULT, SWT.DEFAULT).y);
//	item2.setControl(composite);
//	item2.setImage(image);
//	
//	item0.setExpanded(true);
//	bar.setSpacing(8);
	shell.setSize(400, 350);
	shell.open();
	while (!shell.isDisposed ()) {
		if (!display.readAndDispatch ()) {
			display.sleep ();
		}
	}
//	image.dispose();
	display.dispose();
}

}

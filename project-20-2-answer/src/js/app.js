//----------------------------------------------------------------------------
//
//  JavaScript side of Project 20.1 for "Learning C with Pebble"
//  Uses PebbleKit.js
//
//  Mike Jipping, August, 2016

var debtURL = "http://www.brillig.com/debt_clock";
var populationURL = "http://www.worldometers.info/world-population/us-population";

// Called when the message send attempt succeeds
function messageSuccessHandler() {
  console.log("Message send succeeded.");  
}

// Called when the message send attempt fails
function messageFailureHandler() {
  console.log("Message send failed.");
}

// Goes to "debURL" and scrapes the page for the current US national debt
function get_the_debt() {
   var req = new XMLHttpRequest();  
   req.open('GET', debtURL, false);   
   req.send(null);  
   if(req.status != 200) return "0";
    
   var page = req.responseText;

   // Find the debt number
   var index = page.indexOf("ALT=\"$");
   var pos = page.indexOf("\"", index+6);
   var amount = page.substr(index+6, pos-index-6);
   var regexp = / /g;
   amount = amount.replace(regexp,"");

   return amount;
}

function get_the_population() {
   var req = new XMLHttpRequest();  
   req.open('GET', populationURL, false);   
   req.send(null);  
   if(req.status != 200) return "0";
    
   var page = req.responseText;
   
   var index = page.indexOf("the United States of America");
   var pos = page.indexOf(",", index);
   var pop = page.substr(pos-3, 11);
    
   return pop;
}

// Called when JavaScript is ready
Pebble.addEventListener("ready", function(e) {
  console.log("JS is ready!");
});
												
// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage", function(e) {
  var debt;
  var digits;
  var regexp;
  var dictionary = e.payload;
    
  console.log('Got message: ' + JSON.stringify(dictionary));
  if (dictionary['ASK']) {
        debt = get_the_debt();
        console.log("Debt = " + debt);
        Pebble.sendAppMessage({"DEBT": debt}, messageSuccessHandler, messageFailureHandler);
  } else if (dictionary['RIGHTMOST']) {
      digits = dictionary['RIGHTMOST']+1;  // for the decimal point
      debt = get_the_debt();
      regexp = /,/g;
      debt = debt.replace(regexp,"");
      var rightside = debt.substr(debt.length-digits, digits);
      console.log("Debt is "+debt+" rightmost "+digits+" digits is "+rightside);
      var rightsideint = parseFloat(rightside) * 100;
      Pebble.sendAppMessage({"RIGHTMOST": rightsideint}, messageSuccessHandler, messageFailureHandler);
  } else if (dictionary['LEFTMOST']) {
      digits = dictionary['LEFTMOST'];
      debt = get_the_debt();
      regexp = /,/g;
      debt = debt.replace(regexp,"");
      var leftside = debt.substr(0, digits);
      console.log("Debt is "+debt+" leftmost "+digits+" digits is "+leftside);
      var leftsideint = parseInt(leftside);
      Pebble.sendAppMessage({"LEFTMOST": leftsideint}, messageSuccessHandler, messageFailureHandler);
  } else if (dictionary['USPOPULATION']) {
      debt = get_the_debt();
      var pop = get_the_population();
      regexp = /,/g;
      pop = pop.replace(regexp,"");
      var popint = parseInt(pop);
      console.log("Population is "+popint);
      Pebble.sendAppMessage({"USPOPULATION": popint, "DEBT": debt}, messageSuccessHandler, messageFailureHandler);  
  }
});
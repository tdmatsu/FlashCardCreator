////////////////////////////////////////////////////////////////////////////////
// Declarations                                                               //
////////////////////////////////////////////////////////////////////////////////

function $(e){return document.getElementById(e);}

// XMLHttpRequest object
var ajaxRequest;

var flashcards;
var blnStartup = true;

////////////////////////////////////////////////////////////////////////////////
// Debug utilities
////////////////////////////////////////////////////////////////////////////////
// Debug flag
var DEBUG = false;


var strDebugLog = "";
var firstLog = true;

function clearLog()
{
	strDebugLog = "<hr><a href='javascript:clearLog();'>Clear log</a><br>";
	$('debugOutputArea').innerHTML = strDebugLog;
}

//----------------------------------------------------------------------------------------------------------------------
// printLog
//----------------------------------------------------------------------------------------------------------------------
function _log(str_log)
{
	if (DEBUG == false)
		return;
	
	if (firstLog == true)
	{
		firstLog = false;
		clearLog();
	}
	
	strDebugLog += str_log;
	strDebugLog += "<br/>";
	$('debugOutputArea').innerHTML = strDebugLog;
}

////////////////////////////////////////////////////////////////////////////////
// Goto functions                                                             //
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------
// initOnLoad
//----------------------------------------------------------------------------------------------------------------------
function initOnLoad()
{
	// WRT specific: hide the softkeys
	if (navigator.userAgent.indexOf("Series60/5.0") != -1){
		window.menu.hideSoftkeys();
	}
	
	// initialize the start page
	initStartPage();
	
	// initialize the flashcards page
	flashcards = new FlashCards();
	flashcards.setInstanceName("flashcards");
	flashcards.setRegion($("page_flashcards"));
	
	// go to the start page
	gotoStartPage();
	
	// show/hide debug pane
	$("page_debug").style.display = DEBUG ? "block" : "none";
}

//----------------------------------------------------------------------------------------------------------------------
// initialize the start page
//----------------------------------------------------------------------------------------------------------------------
function initStartPage()
{
	var strwrk = "<SELECT id='selFileName'>";
	for (var i = 0; i < FlashCardCollectionNames.length; i++){
		strwrk = strwrk + "<OPTION value='" + i + "'>" + FlashCardCollectionNames[i] + "<br>";
	}
	strwrk += "</SELECT><br>";
	strwrk += "<input type='button' id='btnStart' value='START!' onclick='javascript:gotoFlashCardPage();'></input><br><br>";
	strwrk += "Key instructions<br>";
	strwrk += "1: next<br>";
	strwrk += "4 or 0: complete the card<br>";
	strwrk += "9: report<br>";
	strwrk += "<br>";
	strwrk += "<input type='checkbox' id='chkAlwaysDone'>Always mark as done</input><br>";
	strwrk += "<input type='checkbox' id='chkShowControls'>Show Controls</input><br>";
	strwrk += "<input type='checkbox' id='chkAutoPlay' onclick='javascript:toggleAutoPlay();'>Autoplay</input>";
	strwrk += "<SELECT id='selSpeed' disabled='true'> ";
	strwrk += "<OPTION value='2000'>SLOW";
	strwrk += "<OPTION SELECTED value='1500'>MODERATE";
	strwrk += "<OPTION value='1000'>FAST";
	strwrk += "<OPTION value='500'>VERY FAST";
	strwrk += "</SELECT><br>";
	
	$("page_start").innerHTML = strwrk;
}

function toggleAutoPlay()
{
	if ($("chkAutoPlay").checked == true){
		$("chkAlwaysDone").checked = true;
		$("chkAlwaysDone").disabled = true;
		$("chkShowControls").checked = false;
		$("chkShowControls").disabled = true;
		$("selSpeed").disabled = false;
	} else {
		$("chkAlwaysDone").disabled = false;
		$("chkShowControls").disabled = false;
		$("selSpeed").disabled = true;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// gotoStartPage
//----------------------------------------------------------------------------------------------------------------------
function gotoStartPage()
{
	$("page_start").style.display = "block";
	$("page_flashcards").style.display = "none";
}

//----------------------------------------------------------------------------------------------------------------------
// gotoFlashCardPage
//----------------------------------------------------------------------------------------------------------------------
function gotoFlashCardPage()
{
	var entries = getFlashCards($("selFileName").value);
	flashcards.setEntries(entries);
	
	flashcards.setAlwaysDone($("chkAlwaysDone").checked);
	flashcards.setAutoPlay($("chkAutoPlay").checked, $("selSpeed").value);
	flashcards.setShowControls($("chkShowControls").checked);
	flashcards.setExitFunction("gotoStartPage()");
	flashcards.startFlashCard();

	$("page_start").style.display = "none";
	$("page_flashcards").style.display = "block";
}

function getFlashCards(index)
{
	 var ret = new Array();
	 for(var i = 0; i < FlashCardCollection[index].length; i++){
	 	ret[i] = FlashCardCollection[index][i];
	}
	return ret;
}


//----------------------------------------------------------------------------------------------------------------------
// keyPress
//----------------------------------------------------------------------------------------------------------------------
// Listener
document.onkeypress = keyPress;

function keyPress(event)
{
	flashcards.handleKey(event.charCode);
}

////////////////////////////////////////////////////////////////////////////////
// Declarations                                                               //
////////////////////////////////////////////////////////////////////////////////

function $(e){return document.getElementById(e);}

var flashCards;

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

//----------------------------------------------------------------------------------------------------------------------
// init
//----------------------------------------------------------------------------------------------------------------------
function init()
{
	// WRT specific: hide the softkeys
	if (navigator.userAgent.indexOf("Series60/5.0") != -1){
		window.menu.hideSoftkeys();
	}

	// initialize the start page
	var strwrk = "";
	for (var i = 0; i < FlashCardCollectionNames.length; i++){
		strwrk = strwrk + "<span id='collection_name'><a href='javascript:startFlashCards(" + i + ");'>" + FlashCardCollectionNames[i] + "</a></span><br>";
		strwrk = strwrk + "<span id='collection_count'>" +  FlashCardCollection[i].length + " entries</span><br><br>";
	}
	$("section_startpage").innerHTML = strwrk;
}


//----------------------------------------------------------------------------------------------------------------------
// Start the flashcards
//----------------------------------------------------------------------------------------------------------------------
function startFlashCards(index)
{
	// copy the entries
	var entries = new Array();
	for(var i = 0; i < FlashCardCollection[index].length; i++){
	 	entries[i] = FlashCardCollection[index][i];
	}

	flashCards = new FlashCards();
	flashCards.setEntries(entries);
	flashCards.setExitFunction("gotoStartPage();");
	flashCards.setRegion($("section_flashcards"));
	flashCards.startFlashCard();
	
	$("section_startpage").style.display = "none";
	$("section_flashcards").style.display = "block";
}

//----------------------------------------------------------------------------------------------------------------------
// gotoStartPage
//----------------------------------------------------------------------------------------------------------------------
function gotoStartPage()
{
	flashCards = null;
	$("section_startpage").style.display = "block";
	$("section_flashcards").style.display = "none";
}

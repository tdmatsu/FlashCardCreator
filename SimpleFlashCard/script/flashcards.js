/*
FlashCards class

** add description here

*/

function FlashCards()
{
	// "constants"
	var LABEL_NEXT = "1.Next";
	var LABEL_DONE = "4.Done";
	
	this.KEY_GONEXT = 49;	// 1 (go next)
	this.KEY_REPORT = 57;	// 9 (report)
	this.KEY_DONE1  = 48;	// 0 (done)
	this.KEY_DONE2  = 52;	// 4 (done)

	var STATE_NOTRUNNING  = -1;
	var STATE_STARTUP     = 0;
	var STATE_LANG1_SHOWN = 1;
	var STATE_BOTH_SHOWN  = 2;
	
	// variables
	var m_entries = null;
	var m_entries_cnt = null;
	var m_blnAlwaysDone = false;
	var m_blnAutoPlay = false;
	var m_intAutoPlayeInterval = 1000;
	var m_blnShowControls = false;
	var m_strInstanceName;
	var m_strExitFunction;
	var m_intState = STATE_NOTRUNNING;
	var m_intCurIndex;
	var m_curCard = {strLang1: "", strLang2: ""};

	this.setAlwaysDone = function(flag){
		m_blnAlwaysDone = flag;
	}
	
	this.setAutoPlay = function(flag, interval){
		m_blnAutoPlay = flag;
		m_intAutoPlayeInterval = interval;
	}

	this.setShowControls = function(flag){
		m_blnShowControls = flag;
	}

	this.setInstanceName = function(str){
		m_strInstanceName = str;
	}
	
	this.setExitFunction = function(str){
		m_strExitFunction = str
	}
	
	this.setRegion = function(region){
		var strwrk = "";
		strwrk += "<div style='height:10%'>";
		strwrk += "<div id='lblInfo' align='right'></div>";
		strwrk += "</div>";
		strwrk += "<div style='height:70%'>";
		strwrk += "<div id='lblLang1'></div><br>";
		strwrk += "<div id='lblLang2'></div><br>";
		strwrk += "</div>";
		strwrk += "<div style='height:20%'>";
		strwrk += "<div id='lblNext'></div>";
		strwrk += "<div id='lblDone'></div>";
		strwrk += "</div>";
		
		region.innerHTML = strwrk;
	}
	
	this.setEntries = function(entries){
		m_entries = entries;
		m_entries_cnt = entries.length;
	}

	this.autoPlayCallback = function()
	{
		this.handleKey(this.KEY_GONEXT);
		if (m_entries.length != 0){
			setTimeout(m_strInstanceName + ".autoPlayCallback();", m_intAutoPlayeInterval);
		}
	}
	
	this.startFlashCard = function()
	{
		_log("startFlashCard()");
		
		m_intState = STATE_STARTUP;
		
		$("lblNext").style.display = m_blnShowControls ? "block" : "none";
		$("lblDone").style.display = m_blnShowControls ? "block" : "none";
	
		this.handleKey(this.KEY_GONEXT);
		
		// if autoplay is enabled, start the timer
		if (m_blnAutoPlay == true){
			setTimeout(m_strInstanceName + ".autoPlayCallback();", m_intAutoPlayeInterval);
		}
	}
	
	this.handleKey = function(chrCode)
	{
		_log("handleKey(" + chrCode + ")");
		_log(m_entries.length + " entries");
		_log("state: " + m_intState);

		if (m_intState == STATE_NOTRUNNING){
			return;
		}
	
		if (m_entries.length == 0){
			return;
		}
	  	
		if (chrCode == this.KEY_GONEXT){ 	// 1: go next  
			if (m_blnAlwaysDone == true && m_intState == STATE_BOTH_SHOWN){
				m_entries.splice(m_intCurIndex,1);
			}
			goNext();
		} else if (chrCode == this.KEY_REPORT){ // 9: report
			document.location.href="mailto:?subject=flashcard:%20" + $("lblLang1").innerHTML;
		} else if ((chrCode == this.KEY_DONE1 || chrCode == this.KEY_DONE2) && m_intState == STATE_BOTH_SHOWN){ // 4, 0: done
			m_entries.splice(m_intCurIndex,1);
			goNext();
		}
	}
	
	var goNext = function()
	{
		_log("goNext");
		
		if (m_intState == STATE_BOTH_SHOWN || m_intState == STATE_STARTUP){
			if (m_entries.length == 0){
				$("lblInfo").innerHTML = "All done! <a href='javascript:" + m_strExitFunction + "'>Return to start page</a>";
				$("lblLang1").innerHTML = "";
				$("lblLang2").innerHTML = "";
				$("lblNext").innerHTML = LABEL_NEXT;
				$("lblDone").innerHTML = LABEL_DONE;
			} else {
				var d = new Date();
				for (var i = 0; i < d.getSeconds(); i++){
					m_intCurIndex = Math.floor(Math.random() * m_entries.length);
				}
				var tmpArray = m_entries[m_intCurIndex].split("\t");
				m_curCard.strLang1 = tmpArray[0];
				m_curCard.strLang2 = tmpArray[1];
				
				// update labels
				$("lblInfo").innerHTML = m_entries.length + "/" + m_entries_cnt + " card(s) left";
				$("lblLang1").innerHTML = m_curCard.strLang1;
				$("lblLang2").innerHTML = "";
				$("lblNext").innerHTML = "<a href='javascript:" + m_strInstanceName + ".handleKey(" + m_strInstanceName + ".KEY_GONEXT);'>" + LABEL_NEXT + "</a>";
				$("lblDone").innerHTML = LABEL_DONE;
				m_intState = STATE_LANG1_SHOWN;
			}
		} else {
			// update labels
			$("lblLang2").innerHTML = m_curCard.strLang2;
			$("lblDone").innerHTML = "<a href='javascript:" + m_strInstanceName + ".handleKey(" + m_strInstanceName + ".KEY_DONE1);'>" + LABEL_DONE + "</a>";
			m_intState = STATE_BOTH_SHOWN;
		}
	}
}


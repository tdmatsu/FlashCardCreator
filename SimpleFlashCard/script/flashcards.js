/*
FlashCards class

** add description here

*/

function FlashCards()
{
	// variables
	var m_entries = null;
	var m_entries_cnt = null;
	var m_strExitFunction;
	var m_intCurIndex;

	this.setExitFunction = function(str){
		m_strExitFunction = str
	}
	
	this.setRegion = function(region){
		var strwrk = "";
		strwrk += "<div id='section_info' align='right'></div><br>";
		strwrk += "<div id='section_cards'></div><br>";
		
		region.innerHTML = strwrk;
	}
	
	this.setEntries = function(entries){
		m_entries = entries;
		m_entries_cnt = entries.length;
	}
	
	var updateCount = function()
	{
		$("section_info").innerHTML = m_entries.length + "/" + m_entries_cnt + " entrie(s) left";
	}
	
	this.startFlashCard = function()
	{
		_log("startFlashCard()");
		
		var parentElm = $("section_cards");
		
		for (var i = 0; i < m_entries.length; i++){
			var elm = document.createElement("div");
			elm.id = "entry_" + i;
			elm.className = "class_entry";

			var elmIndex = document.createElement("div");
			elmIndex.id = elm.id + "_index";
			elmIndex.innerHTML = i;
			elmIndex.style.display = "none";

			var elmText1 = document.createElement("div");
			elmText1.className = "entry_text1";
			elmText1.id = "text1_" + i;
			elmText1.innerHTML = m_entries[i].split("\t")[0];
			elmText1.style.display = "block";
			elmText1.entryIndex = i;
			
			elmText1.onclick = function ()
			{
				if ($("text2_" + this.entryIndex).style.display == "none"){
					$("text2_" + this.entryIndex).style.display = "block";
				}else{
					$("text2_" + this.entryIndex).style.display = "none";
				}
			}

			var elmText2 = document.createElement("div");
			elmText2.className = "entry_text2";
			elmText2.id = "text2_" + i;
			elmText2.innerHTML = m_entries[i].split("\t")[1];
			elmText2.style.display = "none";
			elmText2.entryIndex = i;

			elmText2.onclick = function ()
			{
				$("entry_" + this.entryIndex).style.display = "none";
				m_entries.splice(this.entryIndex, 1);
				updateCount();
			}

			elm.appendChild(elmIndex);
			elm.appendChild(elmText1);
			elm.appendChild(elmText2);
			
			parentElm.appendChild(elm);
		}
		
		updateCount();
	}
}


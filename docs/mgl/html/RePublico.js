//-------------------------------------------------------
// RePublico JavaScipt support routines
// (c) 2001, 2002 RepubliCorp P/L
// Version 1.1
// Author: John Bradnam
//-------------------------------------------------------

//----------------------------------------------
//Generic popup functions
//----------------------------------------------

var m_sErrLinksToObjects = 'Hyperlinks to objects do not work in popups.'

var m_PopupDiv = null;
var m_PopupShadowDiv = null;
var m_PopupTopicDiv = null;
var m_PopupDivStyle = null;
var m_PopupIFrame = null;
var m_PopupIFrameStyle = null;
var m_wPopupClickX = 0;
var m_wPopupClickY = 0;
var m_fPopupTimeoutExpired = false;
var fIE  = (navigator.appName.indexOf("Microsoft") != -1);
var fIE4 = ((fIE) && (parseInt(navigator.appVersion) >= 4));
var fIE6 = ((fIE) && (navigator.userAgent.toLowerCase().indexOf("msie 6.0") != -1));

function RePublicoOnMouseOver(event)
{
  window.m_wPopupMenuX = event.clientX;
  window.m_wPopupMenuY = event.clientY;
  if (!fIE) {
    window.m_wPopupClickX = event.clientX;
    window.m_wPopupClickY = event.clientY;
  }
  else {
    var oBody   = document.body;
    var oBanner = oBody.firstChild;
    var wHeight = 0;
    var wTop    = oBody.scrollTop;
    var wLeft   = oBody.scrollLeft;
    if (oBanner.id == "RpoBanner") {
      wHeight   = oBanner.offsetHeight;
      wTop      = oBanner.nextSibling.scrollTop;
      wLeft     = oBanner.nextSibling.scrollLeft;
    }
    //Record mouse position over link for popup positioning
    window.m_wPopupClickX = event.clientX + wLeft;
    window.m_wPopupClickY = event.clientY + wTop - wHeight;
  }
} //RePublicoOnMouseOver

function RePublicoPopup(sUrl)
{
  //Only work on IE4 and higher for Windows
  if ((fIE4) && (navigator.userAgent.toLowerCase().indexOf("mac") == -1)) {
    if (m_PopupDiv == null) {
      var oTag = document.all.tags("div");
      for (var iDiv = 0; iDiv < oTag.length; iDiv++) {
        if (oTag(iDiv).id == "RePublicoPopup")
          m_PopupDiv       = oTag(iDiv);
        if (oTag(iDiv).id == "RePublicoPopupShadow")
          m_PopupShadowDiv = oTag(iDiv);
        if (oTag(iDiv).id == "RePublicoPopupTopic")
          m_PopupTopicDiv  = oTag(iDiv);
      } //for
      m_PopupIFrame = eval("m_PopupDiv.document.frames['RePublicoPopupIFrameName']");
      m_PopupDivStyle = eval("m_PopupDiv.style");
      m_PopupIFrameStyle = eval("document.all['RePublicoPopupIFrameName'].style");
    }
    if (this.name == "RePublicoPopupIFrameName") {
      //Use existing window
      location.href = sURL;
      parent.window.m_fPopupTimeoutExpired = false;
    } else {
      m_PopupIFrame.location.href = sUrl;
	  window.m_fPopupTimeoutExpired = false;
    }
    setTimeout("_RePublicoPopup()",200);
  }
  else {
    var wX = window.m_wPopupClickX;
    var wY = window.m_wPopupClickY;
    if (!fIE) {
      wX = window.screenX + (window.outerWidth - window.innerWidth) + wX;
      if ((wX + 440) > screen.availWidth)
        wX = screen.availWidth - 440;
      wY = window.screenY + (window.outerHeight - window.innerHeight) + wY;
      if ((wY + 340) > screen.availHeight)
        wY = screen.availHeight - 340;
    }
    var sParam = "titlebar=no,toolbar=no,status=no,location=no,menubar=no,resizable=yes,scrollbars=yes,";
    sParam = sParam + "height=300,width=400";
    sParam = sParam + ",screenX=" + wX + ",screenY=" + wY;
    window.open(sUrl,"",sParam);
  }
} //RePublicoPopup

function _RePublicoPopup()
{
  if ((window.m_PopupIFrame.document.readyState == "complete") && (window.m_PopupIFrame.document.body != null))
    _RePublicoResize();
  else
    setTimeout("_RePublicoPopup()",200);
} //_RePublicoPopup

function _RePublicoResize()
{
  window.m_PopupDivStyle.visibility = "hidden";
  var oSize         = new _RePublicoGetPointObject(0,0);
  _RePublicoGetPopupSize(window.m_PopupIFrame,oSize);
  window.m_PopupDivStyle.width  = oSize.x;
  window.m_PopupDivStyle.height = oSize.y;
  var oPosition     = new _RePublicoGetPointObject(0,0);
  _RePublicoGetPopupPosition(oPosition,oSize);
  window.m_PopupDivStyle.left = oPosition.x;
  window.m_PopupDivStyle.top = oPosition.y;
  window.m_PopupShadowDiv.style.left = 6;
  window.m_PopupShadowDiv.style.top = 6;
  window.m_PopupShadowDiv.style.width = oSize.x;
  window.m_PopupShadowDiv.style.height = oSize.y;
  window.m_PopupTopicDiv.style.width = oSize.x;
  window.m_PopupTopicDiv.style.height = oSize.y;
  window.m_PopupIFrameStyle.width = oSize.x;
  window.m_PopupIFrameStyle.height = oSize.y;
  window.m_PopupDivStyle.visibility = "visible";
  setTimeout("_RePublicoResizeTimeout();", 100);
  return false;
} //_RePublicoResize

function _RePublicoResizeTimeout()
{
  if ((m_PopupIFrame.document.readyState == "complete") && (m_PopupIFrame.document.body != null)) {
    window.m_fPopupTimeoutExpired = true;
    if (m_PopupIFrame.document)
      m_PopupIFrame.document.body.onclick = _RePublicoPopupClicked;
    document.onmousedown = _RePublicoPopupParentClicked;
  }
  else
	setTimeout("_RePublicoResizeTimeout()", 100);
} //_RePublicoResizeTimeout

function _RePublicoGetPointObject(x, y)
{
  this.x = x;
  this.y = y;
} //_RePublicoGetPointObject

function _RePublicoGetPopupSize(oWindow,oSize)
{
  var wClientWidth  = document.body.clientWidth;
  var wClientHeight = document.body.clientHeight;
  var oBody         = oWindow.document.body;
  var wX            = wClientWidth - 20;
  var wY            = 1;
  oWindow.resizeTo(1,1);
  oWindow.resizeTo(1,1);
  oWindow.resizeTo(wX,oBody.scrollHeight);
  oWindow.resizeTo(wX,oBody.scrollHeight);
  oSize.x           = wX;
  oSize.y           = oBody.scrollHeight;
  var wBottom       = 25;
  var wTop          = oSize.x;
  var wMiddle       = 0;
  while (wX != wMiddle) {
    wMiddle         = wX;
    wX              = (wBottom + wTop) >> 1;
    oWindow.resizeTo(wX,oSize.y);
    oWindow.resizeTo(wX,oSize.y);
    if ((oBody.scrollHeight > oSize.y) || (oBody.scrollWidth > wX))
      wBottom       = wX;
    else
      wTop          = wX;
  } //while
  wX                = wBottom;
  oWindow.resizeTo(wX,wY);
  oWindow.resizeTo(wX,wY);
  oSize.x           = oBody.scrollWidth + 20;
  if (oSize.y < 40)
    oSize.y         = 40;
  oWindow.resizeTo(oSize.x,oSize.y);
  oWindow.resizeTo(oSize.x,oSize.y);
  return;
} //_RePublicoGetPopupSize

function _RePublicoGetPopupPosition(oPosition,oSize)
{
  var wClickX = window.m_wPopupClickX;
  var wClickY = window.m_wPopupClickY;
  var oBody   = document.body;
  var wMaxWidth  = oBody.clientWidth + oBody.scrollLeft - 20;
  var wMaxHeight = oBody.clientHeight + oBody.scrollTop - 20;
  if (wClickX + oSize.x < wMaxWidth)
    oPosition.x = wClickX;
  else
    oPosition.x = wMaxWidth - oSize.x - 8;
  if (wClickY + oSize.y + 20 < wMaxHeight)
    oPosition.y = wClickY + 10;
  else
    oPosition.y = wMaxHeight - oSize.y - 20;
  if (oPosition.x < 0) oPosition.x = 1;
  if (oPosition.y < 0) oPosition.y = 1;
} //_RePublicoGetPopupPosition

function _RePublicoPopupParentClicked()
{
  _RePublicoPopupClicked();
  return;
} //_RePublicoPopupParentClicked

function _RePublicoPopupClicked()
{
  var fResult = false;
  if (window.m_fPopupTimeoutExpired) {
    var oEvent = m_PopupIFrame.window.event;
    if ((oEvent != null) &&
        (oEvent.srcElement != null) &&
        (oEvent.srcElement.tagName == "A") &&
        (oEvent.srcElement.href.indexOf("javascript:") == 0) &&
        (oEvent.srcElement.href.indexOf(".") != -1)) {
      //Call through an object error
      m_PopupIFrame.window.event.cancelBubble = true;
      alert(m_sErrLinksToObjects);
    }
    else {
      if (m_PopupIFrame.document)
        m_PopupIFrame.document.body.onclick = null;
      document.onclick = null;
      document.onmousedown = null;
      m_PopupDivStyle.visibility = "hidden";
      fResult = true;
    }
  }
  return fResult;
} //_RePublicoPopupClicked

//----------------------------------------------
//Generic tracking functions
//----------------------------------------------

function RePublicoTrack(sFrameSetUrl,sTocFrameName,fObject)
{
  var sThisUrl = "none";
  var date = new Date();
  date.setYear(date.getYear() + 1);
  if (navigator.cookieEnabled) {
    var aCookie = document.cookie.split("; ");
    for (var i=0; i < aCookie.length; i++) {
      var aCrumb = aCookie[i].split("=");
      if ((aCrumb[0] == "RpoTrack") && (aCrumb[1] != "none")) {
        //Before we access the cookie, check to see if the toc window is present
        try{
          if (fObject == true)
            window.parent.frames[sTocFrameName].document.toc.setCurrent(window.location.href);
          else
            window.parent.frames[sTocFrameName].setCurrent(window.location.href);
          //Will only get here if the toc is loaded and running
          sThisUrl = unescape(aCrumb[1]);
          if (sThisUrl != window.location.href) {
            window.status = "Switching to " + sThisUrl;
            setTimeout("_RePublicoTrackWait('" + sThisUrl + "','" + sTocFrameName + "'," + fObject + ")",1000);
          }
          else
            document.cookie = "RpoTrack=none;path=/;expires=" + date.toGMTString();
        }
        catch(wError) {
          setTimeout("RePublicoTrack('" + sFrameSetUrl + "','" + sTocFrameName + "'," + fObject + ")",250);
          sThisUrl = "";
        }
        break;
      }
    } //for
  }
  if (sThisUrl == "none") {
    if (window.parent.frames[sTocFrameName])
      _RePublicoTrackWait(location.href,sTocFrameName,fObject);
    else {
      if (navigator.cookieEnabled)
        document.cookie = "RpoTrack=" + escape(location.href) + ";path=/;expires=" + date.toGMTString();
      window.location.replace(sFrameSetUrl);
    }
  }
} //RePublicoTrack

function _RePublicoTrackWait(sPageUrl,sTocFrameName,fObject)
{
  //Set the href of the frame and when the TOC is ready, sync it
  try {
    if (fObject == true)
       window.parent.frames[sTocFrameName].document.toc.setCurrent(sPageUrl);
    else
       window.parent.frames[sTocFrameName].setCurrent(sPageUrl);
    if (window.location.href != sPageUrl) {
      window.location.href = sPageUrl;
      throw "error";
    }
    var date = new Date();
    date.setYear(date.getYear() - 1);
    if (navigator.cookieEnabled)
      document.cookie = "RpoTrack=none;path=/;expires=" + date.toGMTString();
    window.status = "Done";
  }
  catch(wError) {
    setTimeout("_RePublicoTrackWait('" + sPageUrl + "','" + sTocFrameName + "'," + fObject + ")",500);
  }
} //_RePublicoTrackWait

//----------------------------------------------
//Generic show/hide division functions
//----------------------------------------------

var sVisibleGif = "arrowdn.gif";
var sHiddenGif  = "arrowrt.gif";

function _RePublicoMouseOnShowHide() {
  var oPar = event.srcElement;
  if (oPar.tagName == "IMG")
    oPar = oPar.parentElement;
  if (event.type == "mouseover")
    oPar.style.fontWeight = "bold";
  else
    oPar.style.fontWeight = "normal";
} //_RePublicoMouseOnShowHide
    
function _RePublicoClickShowHide() {
  var oPar = event.srcElement;
  var oImg = null;
  if (oPar.tagName == "A") {
    oImg = oPar.firstChild;
    oPar = oPar.parentElement;
  }
  else if (oPar.tagName == "IMG") {
    oImg = oPar;
    oPar = oPar.parentElement;
    if (oPar.tagName == "A")
      oPar = oPar.parentElement;
  }
  else {
    oImg = oPar.firstChild;
    if (oImg.tagName == "A")
      oImg = oImg.firstChild;
  }
  while ((oImg.nextSibling != null) && (oImg.nextSibling.tagName == "IMG"))
    oImg = oImg.nextSibling;
  var oDiv = oPar.nextSibling;
  if (oDiv.style.display == "none") {
    oDiv.style.display = "";
    oImg.src = "./Images/" + sVisibleGif;
  }
  else {
    oDiv.style.display = "none";
    oImg.src = "./Images/" + sHiddenGif;
  }
} //_RePublicoClickShowHide

function RePublicoShowHide() {
  var sFile   = "";
  var oPar    = null;
  var oDiv    = null;
  var wIndex  = 0;
  var oImgCol = document.all.tags("img");
  if (oImgCol != null) {
    for (wIndex = 0;wIndex < oImgCol.length; wIndex++) {
      oImg = oImgCol[wIndex];
      sFile = oImg.src.substr(oImg.src.lastIndexOf("/")+1);
      if ((sFile == sHiddenGif) || (sFile == sVisibleGif)) {
        oPar = oImg.parentElement;
        if (oPar.tagName == "A")
          oPar = oPar.parentElement;
        if ((oPar != null) && (oPar.tagName == "P")) {
          oDiv = oPar.nextSibling;
          if (oDiv != null) {
            if (sFile == sHiddenGif)
              oDiv.style.display = "none";
            else
              oDiv.style.display = "";
            oPar.attachEvent('onmouseover',_RePublicoMouseOnShowHide);
            oPar.attachEvent('onmouseout',_RePublicoMouseOnShowHide);
            oPar.attachEvent('onclick',_RePublicoClickShowHide);
            oPar.style.cursor = "hand";
          }
        }
      }
    } //for
  }
} //RePublicoShowHide

//----------------------------------------------
//Generic title area
//----------------------------------------------

function RePublicoBanner(wLines) {
  var oRest   = null;
  var oNext   = null;
  var oNsr    = null;
  var oScroll = null;
  var wHeight = 0;
  var wTopPad = 6;
  var wBotPad = 0;

  var oBody   = document.body;
  var oBanner = oBody.firstChild;
  if ((oBanner != null) && (wLines != 0)) {
    oNsr                = document.createElement("div");
    oNsr.id             = "RpoBanner";
    oNsr.style.width    = oBody.clientWidth;
    oNsr.style.height   = 0;
    oNsr.style.overflow = "hidden";
    oBanner.insertAdjacentElement("beforeBegin",oNsr);
    wTopPad             = 6;
    while ((oBanner != null) && (wLines != 0)) {
      if ((oBanner.tagName == "DIV") && (oBanner.className.toLowerCase().indexOf("section") != -1)) {
        oNext   = oBanner.firstChild;
        oBanner.removeNode(false);
        oBanner = oNext;
      }
      if (oBanner.tagName == "TABLE") {
        wHeight = wHeight + oBanner.scrollHeight + 2;
        wTopPad = 6;
      }
      else {
        oBanner.style.margin = "0";
        oBanner.style.textIndent = "0";
        if ((wLines == 1) || 
            ((oBanner.nextSibling != null) && (oBanner.nextSibling.tagName == "TABLE")))
          wBotPad = 6;
        else
          wBotPad = 0;
        oBanner.style.padding = wTopPad + "px,6px," + wBotPad + "px,6px";
        if (wLines == 1)
          oBanner.style.borderBottom = "solid 1pt black";
        wHeight = wHeight + oBanner.scrollHeight;
        wTopPad = 0;
      }
      oBanner   = oBanner.nextSibling;
      oNsr.appendChild(oNsr.nextSibling.removeNode(true));
      wLines  = wLines - 1;
    } //while
    oNsr.style.height            = wHeight;
    oRest                        = oNsr.nextSibling;
    if (oRest != null) {
      oBody.style.overflow       = "hidden";
      oScroll = document.createElement("div");
      oScroll.style.width        = oBody.style.width;
      oScroll.style.padding      = "5px";
      oScroll.style.overflowX    = "auto";
      oScroll.style.overflowY    = "scroll";
      oBody.style.margin         = "0";
      oRest.insertAdjacentElement("beforeBegin",oScroll);
      while (oScroll.nextSibling != null)
        oScroll.appendChild(oScroll.nextSibling.removeNode(true));
      window.attachEvent('onresize',_RePublicoOnResizeBody);
      _RePublicoOnResizeBody();
    }
  }
} //XdkBanner


function _RePublicoOnResizeBody() {
  var oBody   = document.body;
  var oBanner = oBody.firstChild;
  var oScroll = oBanner.nextSibling;
  if (oBody.clientHeight > oBanner.clientHeight)
    oScroll.style.height = oBody.clientHeight - oBanner.clientHeight - 1;
  else
    oScroll.style.height = 0;
  oScroll.style.width    = oBody.clientWidth;
  oBanner.style.width    = oBody.clientWidth;
} //_RePublicoOnResizeBody

//----------------------------------------------
//Generic search highlighting
//----------------------------------------------

function RePublicoSelectWords(sWords, fOn) {
  if( sWords!=null && sWords!="" ) {
    //Show all hidden text
    var wIndex = 0;
    var oDiv   = document.all.tags("DIV");
    if (oDiv != null) {
      for (wIndex = 0;wIndex < oDiv.length;wIndex++) {
        if ((oDiv[wIndex].style != null) && (oDiv[wIndex].style.display == "none")) {
          oDiv[wIndex].style.visibility = "hidden";
          oDiv[wIndex].style.display = "";
          oDiv[wIndex].setAttribute("RePublico","search",0);
        }
      } //for
    }
    // word delimeters
    var aWords = sWords.split(sOrAnd);
    var i = 0;
    var fFirst = true;
    for( i in aWords ) {
      var sWord = aWords[i];
      var oRange = window.top.frames(sTocTarget).document.body.createTextRange();
      while( oRange.findText(sWord) ) {
        if (fOn == true) {
          oRange.execCommand("BackColor",false,sSelBack);
          oRange.execCommand("ForeColor",false,sSelFore);
          if (fFirst == true)
            oRange.scrollIntoView();
          fFirst = false;
        }
        else
          oRange.execCommand("RemoveFormat",false,null);
        oRange.collapse(false);
      } //while
    }
    //Hide all hidden text
    if (oDiv != null) {
      for (wIndex = 0;wIndex < oDiv.length;wIndex++) {
        if (oDiv[wIndex].getAttribute("RePublico",0) == "search") {
          oDiv[wIndex].style.display = "none";
          oDiv[wIndex].style.visibility = "visible";
          oDiv[wIndex].removeAttribute("RePublico",0);
        }
      } //for
    }
  }
} //RePublicoSelectWords

//----------------------------------------------
//Generic print functions
//----------------------------------------------

function RePublicoOnBeforePrint() {
  var wIndex = 0;
  var oDiv   = document.all.tags("DIV");
  if (oDiv != null) {
    for (wIndex = 0;wIndex < oDiv.length;wIndex++) {
      if ((oDiv[wIndex].style != null) && (oDiv[wIndex].style.display == "none")) {
        oDiv[wIndex].style.display = "";
        oDiv[wIndex].setAttribute("RePublico","print",0);
      }
    } //for
  }
  //Disable banner
  var oBody = document.body;
  var oBanner = oBody.firstChild;
  if (oBanner.id == "RpoBanner") {
    var oScroll = oBanner.nextSibling;
    oBody.style.overflow = "visible";
    oBanner.style.overflow = "visible";
    oScroll.style.overflow = "visible";
    oScroll.style.height = "auto"
    oScroll.style.width  = "auto";
  }
} //RePublicoOnBeforePrint
        
function RePublicoOnAfterPrint() {
  var wIndex = 0;
  var oDiv   = document.all.tags("DIV");
  if (oDiv != null) {
    for (wIndex = 0;wIndex < oDiv.length;wIndex++) {
      if (oDiv[wIndex].getAttribute("RePublico",0) == "print") {
        oDiv[wIndex].style.display = "none";
        oDiv[wIndex].removeAttribute("RePublico",0);
      }
    } //for
  }
  //Enable banner
  var oBody = document.body;
  var oBanner = oBody.firstChild;
  if (oBanner.id == "RpoBanner") {
    var oScroll = oBanner.nextSibling;
    oBody.style.overflow = "hidden";
    oBanner.style.overflow = "hidden";
    oScroll.style.overflow = "visible";
    oScroll.style.overflowX = "auto";
    oScroll.style.overflowY = "scroll";
    _RePublicoOnResizeBody();
  }
} //RePublicoOnAfterPrint

//-------------- end of RePublico.js --------------------------

//-------------------------------------------------------
// RePublico JavaScipt Toolbar routines
// (c) 2002, RepubliCorp P/L
// Version 1.2
// Author: John Bradnam
//-------------------------------------------------------

var fIE  = (navigator.appName.indexOf("Microsoft") != -1);
var fIE4 = ((fIE) && (parseInt(navigator.appVersion) >= 4));
var fIE6 = ((fIE) && (navigator.userAgent.toLowerCase().indexOf("msie 6.0") != -1));

function buttonSetGraphic(oTable,sImage) {
  if ((fIE) && (!fIE6))
    oTable.firstChild.firstChild.firstChild.firstChild.src = oTable.getAttribute(sImage);
  else {
    var oImg = oTable.rows.item(0).cells.item(0).firstChild;
    while ((oImg != null) && (oImg.nodeName != "IMG"))
      oImg = oImg.nextSibling;
    if (oImg != null)
      oImg.setAttribute("src",oTable.getAttribute(sImage));
  }
} //buttonSetGraphic

function mouseoverButton(oTable) {
  if (oTable.getAttribute("href") != null) {
    if (oTable.className != "buttonDown")
      oTable.className = "buttonOver";
    buttonSetGraphic(oTable,"onimage");
  }
} //mouseoverButton

function mouseoutButton(oTable) {
  if (oTable.getAttribute("href") != null) {
    oTable.className = "buttonOff";
    buttonSetGraphic(oTable,"offimage");
  }
} //mouseoutButton

function mousedownButton(oTable) {
  if (oTable.getAttribute("href") != null)
    oTable.className = "buttonPress";
} //mousedownButton

function mouseupButton(oTable) {
  if (oTable.getAttribute("href") != null) {
    oTable.className = "buttonOver";
    eval(oTable.getAttribute("href"));
  }
} //mouseupButton

function executeButton(sUrl,sTarget) {
  if ((sUrl == null) || (sUrl == ''))
    ;
  else if ((sTarget == null) || (sTarget == ''))
    location.href = sUrl;
  else if (fIE)
    window.top.frames(sTarget).navigate(sUrl);
  else
    window.top.frames[sTarget].location.href = sUrl;
} //executeButton

function executeHistory(sTarget,wDirection) {
  if (!fIE)
    window.top.frames[sTarget].history.go(wDirection);
  else if (wDirection == 1)
    window.top.frames(sTarget).history.forward();
  else
    window.top.frames(sTarget).history.back();
} //executeHistory

//---------------- End of RePublicoToolbar.js ----------------------

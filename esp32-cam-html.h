static const char PROGMEM INDEX_HTML[] = R"rawliteral(
  <!DOCTYPE html>
  <head>
  <title>Text recognition (tesseract.js)</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <script src="https:\/\/ajax.googleapis.com/ajax/libs/jquery/1.8.0/jquery.min.js"></script>
  <script src='https:\/\/unpkg.com/tesseract.js@v2.1.0/dist/tesseract.min.js'></script>
  </head>
  <body>
  <img id="ShowImage" src="">
  <table>
  <tr>
    <td><input type="button" id="restart" value="Restart"></td> 
    <td colspan="2"><input type="button" id="getStill" value="Get Still"></td> 
  </tr>  
  <tr>
    <td>Flash</td>
    <td colspan="2"><input type="range" id="flash" min="0" max="255" value="0"></td>
  </tr>
  <tr>
    <td>Quality</td>
    <td colspan="2"><input type="range" id="quality" min="10" max="63" value="10"></td>
  </tr>
  <tr>
    <td>Brightness</td>
    <td colspan="2"><input type="range" id="brightness" min="-2" max="2" value="0"></td>
  </tr>
  <tr>
    <td>Contrast</td>
    <td colspan="2"><input type="range" id="contrast" min="-2" max="2" value="0"></td>
  </tr>
  <tr>
    <td>Resolution</td> 
    <td colspan="2">
    <select id="framesize">
        <option value="UXGA">UXGA(1600x1200)</option>
        <option value="SXGA">SXGA(1280x1024)</option>
        <option value="XGA">XGA(1024x768)</option>
        <option value="SVGA">SVGA(800x600)</option>
        <option value="VGA">VGA(640x480)</option>
        <option value="CIF">CIF(400x296)</option>
        <option value="QVGA" selected="selected">QVGA(320x240)</option>
        <option value="HQVGA">HQVGA(240x176)</option>
        <option value="QQVGA">QQVGA(160x120)</option>
    </select> 
    </td>
  </tr>      
  <tr>
    <td>Rotate</td>
    <td align="left" colspan="2">
        <select onchange="document.getElementById('canvas').style.transform='rotate('+this.value+')';">
          <option value="0deg">0deg</option>
          <option value="90deg">90deg</option>
          <option value="180deg">180deg</option>
          <option value="270deg">270deg</option>
        </select>
    </td>
  </tr>  
  </table>
  <br>
  <select id="lang">
    <option value='afr'>Afrikaans</option>
    <option value='amh'>Amharic</option>
    <option value='ara'>Arabic</option>
    <option value='asm'>Assamese</option>
    <option value='aze'>Azerbaijani</option>
    <option value='aze_cyrl'>Azerbaijani - Cyrillic</option>
    <option value='bel'>Belarusian</option>
    <option value='ben'>Bengali</option>
    <option value='bod'>Tibetan</option>
    <option value='bos'>Bosnian</option>
    <option value='bul'>Bulgarian</option>
    <option value='cat'>Catalan; Valencian</option>
    <option value='ceb'>Cebuano</option>
    <option value='ces'>Czech</option>
    <option value='chi_sim'>Chinese - Simplified</option>
    <option value='chi_tra'>Chinese - Traditional</option>
    <option value='chr'>Cherokee</option>
    <option value='cym'>Welsh</option>
    <option value='dan'>Danish</option>
    <option value='deu'>German</option>
    <option value='dzo'>Dzongkha</option>
    <option value='ell'>Greek, Modern (1453-)</option>
    <option value='eng' selected>English</option>
    <option value='enm'>English, Middle (1100-1500)</option>
    <option value='epo'>Esperanto</option>
    <option value='est'>Estonian</option>
    <option value='eus'>Basque</option>
    <option value='fas'>Persian</option>
    <option value='fin'>Finnish</option>
    <option value='fra'>French</option>
    <option value='frk'>German Fraktur</option>
    <option value='frm'>French, Middle (ca. 1400-1600)</option>
    <option value='gle'>Irish</option>
    <option value='glg'>Galician</option>
    <option value='grc'>Greek, Ancient (-1453)</option>
    <option value='guj'>Gujarati</option>
    <option value='hat'>Haitian; Haitian Creole</option>
    <option value='heb'>Hebrew</option>
    <option value='hin'>Hindi</option>
    <option value='hrv'>Croatian</option>
    <option value='hun'>Hungarian</option>
    <option value='iku'>Inuktitut</option>
    <option value='ind'>Indonesian</option>
    <option value='isl'>Icelandic</option>
    <option value='ita'>Italian</option>
    <option value='ita_old'>Italian - Old</option>
    <option value='jav'>Javanese</option>
    <option value='jpn'>Japanese</option>
    <option value='kan'>Kannada</option>
    <option value='kat'>Georgian</option>
    <option value='kat_old'>Georgian - Old</option>
    <option value='kaz'>Kazakh</option>
    <option value='khm'>Central Khmer</option>
    <option value='kir'>Kirghiz; Kyrgyz</option>
    <option value='kor'>Korean</option>
    <option value='kur'>Kurdish</option>
    <option value='lao'>Lao</option>
    <option value='lat'>Latin</option>
    <option value='lav'>Latvian</option>
    <option value='lit'>Lithuanian</option>
    <option value='mal'>Malayalam</option>
    <option value='mar'>Marathi</option>
    <option value='mkd'>Macedonian</option>
    <option value='mlt'>Maltese</option>
    <option value='msa'>Malay</option>
    <option value='mya'>Burmese</option>
    <option value='nep'>Nepali</option>
    <option value='nld'>Dutch; Flemish</option>
    <option value='nor'>Norwegian</option>
    <option value='ori'>Oriya</option>
    <option value='pan'>Panjabi; Punjabi</option>
    <option value='pol'>Polish</option>
    <option value='por'>Portuguese</option>
    <option value='pus'>Pushto; Pashto</option>
    <option value='ron'>Romanian; Moldavian; Moldovan</option>
    <option value='rus'>Russian</option>
    <option value='san'>Sanskrit</option>
    <option value='sin'>Sinhala; Sinhalese</option>
    <option value='slk'>Slovak</option>
    <option value='slv'>Slovenian</option>
    <option value='spa'>Spanish; Castilian</option>
    <option value='spa_old'>Spanish; Castilian - Old</option>
    <option value='sqi'>Albanian</option>
    <option value='srp'>Serbian</option>
    <option value='srp_latn'>Serbian - Latin</option>
    <option value='swa'>Swahili</option>
    <option value='swe'>Swedish</option>
    <option value='syr'>Syriac</option>
    <option value='tam'>Tamil</option>
    <option value='tel'>Telugu</option>
    <option value='tgk'>Tajik</option>
    <option value='tgl'>Tagalog</option>
    <option value='tha'>Thai</option>
    <option value='tir'>Tigrinya</option>
    <option value='tur'>Turkish</option>
    <option value='uig'>Uighur; Uyghur</option>
    <option value='ukr'>Ukrainian</option>
    <option value='urd'>Urdu</option>
    <option value='uzb'>Uzbek</option>
    <option value='uzb_cyrl'>Uzbek - Cyrillic</option>
    <option value='vie'>Vietnamese</option>
    <option value='yid'>Yiddish</option>
  </select><br>
  <button id="btnDetect" onclick="TextRecognition();" disabled>Text recognition</button>
  <br>
  <div id="result" style="width:320px;color:red"></div>
  
  <script>
    var getStill = document.getElementById('getStill');
    var ShowImage = document.getElementById('ShowImage');
    var result = document.getElementById('result');
    var flash = document.getElementById('flash');
    var btnDetect = document.getElementById('btnDetect');
    var myTimer;
    var restartCount=0;    

    getStill.onclick = function (event) {  
      clearInterval(myTimer);  
      myTimer = setInterval(function(){error_handle();},5000);
      ShowImage.src=location.origin+'/?getstill='+Math.random();      
    }

    function error_handle() {
      btnDetect.disabled =true;
      restartCount++;
      clearInterval(myTimer);
      if (restartCount<=2) {
        result.innerHTML = "Get still error. <br>Restart ESP32-CAM "+restartCount+" times.";
        myTimer = setInterval(function(){getStill.click();},10000);
      }
      else
        result.innerHTML = "Get still error. <br>Please close the page and check ESP32-CAM.";
    }  

    ShowImage.onload = function (event) {
      btnDetect.disabled =false;
      clearInterval(myTimer);
      restartCount=0; 
      getStill.click();
    }         
    
    restart.onclick = function (event) {
      fetch(location.origin+'?restart=stop');
    }    

    framesize.onclick = function (event) {
      fetch(document.location.origin+'?framesize='+this.value+';stop');
    }  

    flash.onchange = function (event) {
      fetch(location.origin+'?flash='+this.value+';stop');
    } 

    quality.onclick = function (event) {
      fetch(document.location.origin+'?quality='+this.value+';stop');
    } 

    brightness.onclick = function (event) {
      fetch(document.location.origin+'?brightness='+this.value+';stop');
    } 

    contrast.onclick = function (event) {
      fetch(document.location.origin+'?contrast='+this.value+';stop');
    }                             
    
    function getFeedback(target) {
      var data = $.ajax({
      type: "get",
      dataType: "text",
      url: target,
      success: function(response)
        {
          result.innerHTML = response;
        },
        error: function(exception)
        {
          result.innerHTML = 'fail';
        }
      });
    }   

    function TextRecognition() {
      result.innerHTML = "Recognize...";
      
      Tesseract.recognize(
        ShowImage,
        lang.value,
        { logger: m => console.log(m) }
      ).then(({ data: { text } }) => {
  
        result.innerHTML = text.replace(/\n/g, "<br>");
        text = result.innerHTML.replace(/<br>/g, "");
        
      })  
    }     
  </script>
  </body>
  </html> 
)rawliteral";
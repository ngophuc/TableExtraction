# TableExtraction

This is a tool to extract tables in document images. The method is composed of 6 steps:
1. Line segment detection
2. Horizontal and vertical segment filtering
3. Line segment recovery
4. Suppression of segments belonging to text
5. Table cell extraction
6. Table reconstruction

# Quick setup 

* Requires OpenCv library.
* CMakeLists.txt provided for cmake.

To install the program see <a href="https://github.com/ngophuc/TableExtraction/blob/main/Install.txt">Install.txt</a> file

# Examples
<p>Result for <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/eu-002_page0.png">eu-002_page0.png</a>: </p>&#x000A;&#x000A;
<pre class="code highlight js-syntax-highlight plaintext">
<code>./TableExtraction -i ../Samples/eu-002_page0.png -o eu-002_page0_res.png -s 2</code>
</pre>&#x000A;&#x000A;
<p>
  <table cellpadding="5">
    <tr>
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/eu-002_page0.png">
        <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Samples/eu-002_page0.png" alt="Input image" />
      </a>  
    <br />
    Input image
    </td>
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_Seg.png">
        <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_Seg.png" alt="FBSD detector" />
      </a>
    <br />
    FBSD detector
    </td>  
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVEg.png">
        <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVEg.png" alt="Filtering horizontal and vertical segments" />
      </a>
    <br />
    Filtering and recovering segments
    </td>
    </tr>
    <tr>
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVT.png">
        <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVT.png" alt="Removing text segments" />
      </a>  
    <br />
    Removing text segments
    </td> 
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVTable.png">
        <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVTable.png" alt="Table extraction" />
      </a>
    <br />
    Table extraction
    </td>
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/eu-002_page0_res.png">
        <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/eu-002_page0_res.png" alt="Output image" />
      </a>
    <br />
    Output image
    </td>    
    </tr>
  </table>
</p>

<p>Result for <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/eu-001_page0.png">eu-001_page0.png</a>: </p>&#x000A;&#x000A;
<pre class="code highlight js-syntax-highlight plaintext">
<code>./TableExtraction -i ../Samples/eu-001_page0.png -o eu-001_page0_res.png -s 2</code>
</pre>&#x000A;&#x000A;
<p>
<table cellpadding="5">
  <tr>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/eu-001_page0.png">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Samples/eu-001_page0.png" alt="Input image" />
    </a>  
  <br />
  Input image
  </td>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/eu-001_page0_res.png">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/eu-001_page0_res.png" alt="Output image" />
    </a>
  <br />
  Output image
  </td>    
  </tr>
</table>

<p>Result for <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/us-001_page0.png">us-001_page0.png</a>: </p>&#x000A;&#x000A;
<pre class="code highlight js-syntax-highlight plaintext">
<code>./TableExtraction -i ../Samples/us-001_page0.png -o us-001_page0_res.png -s 2</code>
</pre>&#x000A;&#x000A;
<p>
<table cellpadding="5">
  <tr>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/us-001_page0.png">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Samples/us-001_page0.png" alt="Input image" />
    </a>  
  <br />
  Input image
  </td>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/us-001_page0_res.png">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/us-001_page0_res.png" alt="Output image" />
    </a>
  <br />
  Output image
  </td>    
  </tr>
</table>

<p>Result for <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/1_301.jpg">1_301.jpg</a>: </p>&#x000A;&#x000A;
<pre class="code highlight js-syntax-highlight plaintext">
<code>./TableExtraction -i ../Samples/1_301.jpg -o 1_301_res.png</code>
</pre>&#x000A;&#x000A;
<p>
<table cellpadding="5">
  <tr>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/1_301.jpg">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Samples/1_301.jpg" alt="Input image" />
    </a>  
  <br />
  Input image
  </td>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/1_301_res.png">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/1_301_res.png" alt="Output image" />
    </a>
  <br />
  Output image
  </td>    
  </tr>
</table>

# Limit cases

<p>Result for <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/10.1.1.1.2111_7.jpg">10.1.1.1.2111_7.jpg</a>: </p>&#x000A;&#x000A;
<pre class="code highlight js-syntax-highlight plaintext">
<code>./TableExtraction -i ../Samples/10.1.1.1.2111_7.jpg -o 10.1.1.1.2111_7_res.png</code>
</pre>&#x000A;&#x000A;
<p>
<table cellpadding="5">
  <tr>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/10.1.1.1.2111_7.jpg">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Samples/10.1.1.1.2111_7.jpg" alt="Input image" />
    </a>  
  <br />
  Input image
  </td>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/10.1.1.1.2111_7_res.png">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/10.1.1.1.2111_7_res.png" alt="Output image" />
    </a>
  <br />
  Output image (graphics are  mistakenly recognized as tables)
  </td>    
  </tr>
</table>

<p>Result for <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/1078_082.png">1078_082.png</a>: </p>&#x000A;&#x000A;
<pre class="code highlight js-syntax-highlight plaintext">
<code>./TableExtraction -i ../Samples/1078_082.png -o 1078_082_res.png</code>
</pre>&#x000A;&#x000A;
<p>
<table cellpadding="5">
  <tr>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Samples/1078_082.png">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Samples/1078_082.png" alt="Input image" />
    </a>  
  <br />
  Input image
  </td>
  <td align="center" valign="center">
    <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/1078_082_res.png">
      <img width="250" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/1078_082_res.png" alt="Output image" />
    </a>
  <br />
  Output image (Boundless table non detected)
  </td>    
  </tr>
</table>

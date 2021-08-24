# TableExtraction

This is a tool to extract tables in document images.

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
        <img width="150" src="https://github.com/ngophuc/TableExtraction/blob/main/Samples/eu-002_page0.png" alt="Input image" />
      </a>  
    <br />
    Input image
    </td>
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_Seg.png">
        <img width="150" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_Seg.png" alt="FBSD detector" />
      </a>
    <br />
    FBSD detector
    </td>  
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/ModifiedAdaptiveTagentialCover/blob/master/Results/bird5_DPnew_ATC.pdf">
        <img width="150" src="https://github.com/ngophuc/ModifiedAdaptiveTagentialCover/blob/master/Results/bird5_DPnew_ATC.png" alt="Polygonal approximation with ATC" />
      </a>
    <br />
    Polygonalization with ATC
    </td>    
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HV.png">
        <img width="150" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HV.png" alt="Filtering horizontal and vertical segments" />
      </a>
    <br />
    Filtering horizontal and vertical segments
    </td>
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVEg.png">
        <img width="150" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVEg.png" />
      </a>
    <br />
    Recovering segments
    </td>    
    </tr>
  </table>
</p>

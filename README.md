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
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVEg.png">
        <img width="150" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVEg.png" alt="Filtering horizontal and vertical segments" />
      </a>
    <br />
    Filtering and recovering segments
    </td>
    </tr>
    <tr>
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVT.png">
        <img width="150" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVT.png" alt="Removing text segments" />
      </a>  
    <br />
    Removing text segments
    </td> 
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVTable.png">
        <img width="150" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/res_HVTable.png" alt="Table extraction" />
      </a>
    <br />
    Table extraction
    </td>
    <td align="center" valign="center">
      <a href="https://github.com/ngophuc/TableExtraction/blob/main/Results/eu-002_page0_res.png">
        <img width="150" src="https://github.com/ngophuc/TableExtraction/blob/main/Results/eu-002_page0_res.png" alt="Output image" />
      </a>
    <br />
    Output image
    </td>    
    </tr>
  </table>
</p>

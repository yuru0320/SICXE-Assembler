# SICXE-ssembler

## 開發平台
macOS Big Sur 
## 使用開發環境
Xcode v12.5.1 (12E507) 

## 程式功能
將切好的token依照對應的方法翻成機器碼。

## 程式流程 
* 首先輸入1，若是要結束程式則輸入0。 
* 再輸入檔名ex: input，若檔案不存在則需重新輸入到正確為止。
* 將input檔翻譯完成後會將結果寫到一個新的output檔後輸出。

output檔包含：Line(行數), Loc(位置), Source statement, Object code。
## 程式說明
### SIC: 
將label與label location儲存在label table中，將對應指令的opcode及symbol的location結合組成機器碼。
* Table1.table(instruction table)
* Table2.table(pseudo table)
* Table3.table(register table)
* Table4.table(delimeter table)

若symbol為unfined symbol就在讀完全部檔案後，印出結果前，針對machine還未翻譯完成的指令再進行一次對應指令的opcode及symbol的location結合。

再翻成機器碼前要先進行語法檢查，若是有不符合文法的地方 就印出error後繼續執行下一個指令。 

### SICXE:
將label與label location儲存在label table中，針對指令判斷為哪種format。
* format1為直接印出opcode 即可。
* format2為opcode r1, r2。
* format3為opcode n, i, x, b, p, e, disp。
* format4為opcode n, i, x, b, p, e, address 。

依照format的格式計算出最後的機器碼。

完成的pseudo instruction：START、END、BYTE、WORD、RESB、RESW、EQU、BASE、LTORG。



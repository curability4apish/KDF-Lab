
## 為什麼要使用密碼管理工具 Why IsaacVault?
我可以不使用密碼管理工具，反而使用幾組自己能記得的密碼就好了嗎？你可以這麼做，不過當你所註冊的平台發生大規模的[資料外洩](https://haveibeenpwned.com/)，你用相同的信箱與密碼所註冊的其他平台也將面臨遭駭的風險。

Instead of a password manager, why not just use several memorable passwords? Of course you can, but when the platforms you have registered happen to breach data due to cyber security flaws, and that you also register another platform with the same email and password, those account have risks of being pwned.
##### 我可以不使用密碼管理工具，反而使用筆記軟體或文字檔記錄自己的密碼嗎？

你可以這麼做，不過多一層密碼保護你所記錄密碼的工具，能避免間諜軟體之類未經授權的存取或系統性漏洞造成的密碼外洩風險。
## 為什麼要使用 IsaacVault
##### 零足跡
它透過 [ISAAC](https://en.wikipedia.org/wiki/ISAAC_(cipher)) [安全亂數產生器](https://zh.wikipedia.org/zh-tw/%E5%AF%86%E7%A0%81%E5%AD%A6%E5%AE%89%E5%85%A8%E4%BC%AA%E9%9A%8F%E6%9C%BA%E6%95%B0%E7%94%9F%E6%88%90%E5%99%A8)的驅動與你的 mainkey、sitekey 為你查詢安全的密碼。因此它不需要網路權限，也不會儲存任何密碼到裝置，以此確保安全性。
##### 無網路權限
如果你注重密碼跨裝置同步的方便性，你可以選擇線上密碼管理工具。如果你更注重密碼的控制權，或者對任何線上密碼管理提供商的隱私政策或安全性保持疑慮，也許 IsaacVault 就是你正在找的。
##### 零知識密碼生成與管理
它根據你的 mainkey 與 sitekey 來幫你查找安全的密碼。因此除了你，沒有人可以用任何方式知道你的密碼，但除非有人知道你的 mainkey、sitekey，並知道你的密碼管理工具是     IsaacVault。
##### 能抵禦暴力、統計式攻擊
IsaacVault 透過 [ISAAC](https://en.wikipedia.org/wiki/ISAAC_(cipher)) [安全亂數產生器](https://zh.wikipedia.org/zh-tw/%E5%AF%86%E7%A0%81%E5%AD%A6%E5%AE%89%E5%85%A8%E4%BC%AA%E9%9A%8F%E6%9C%BA%E6%95%B0%E7%94%9F%E6%88%90%E5%99%A8)的驅動與你的 mainkey、sitekey 為你查詢安全的密碼。由於 ISAAC 具有不低於 [2^40 的週期](https://www.burtleburtle.net/bob/rand/isaacafa.html)，如果任何人不是同時知道你的 mainkey 與 sitekey，就無法同時知道你為該網站平台所設的密碼。任何人如果只知道你的一個網站平台的密碼，他無法透過任何方法計算出你的 mainkey 與 sitekey。


## 使用步驟
##### 設定你的主金鑰
預設10位的 ASCII(32~126)。若遺失，你將無法查詢密碼，故請妥站保存。
##### 輸入你的網站金鑰
可為 網站名稱、應用程式包名 或 網站域名
##### 找到安全的密碼
隨即查到密碼 ASCII(32-126)、建議的使用者名稱 ASCII(48-57、65-90、97-122)

![](https://megapx-assets.dcard.tw/images/a7fe32b7-f432-46b5-9533-6f845803f46d/1280.webp)

## 未來目標
這些目標目前尚無法達成，但期待自己有朝一日可以達成。
- [ ] 擴充功能：自動填入網站域名作為 sitekey，並具備自動填入表單，並立即清除瀏覽器的表單填寫紀錄
- [ ] 在程式中配置並加密 mainkey，同時能執行程式，兼顧方便與安全性
- [ ] Telegram Bot
- [ ] 透過密碼學分析進一步檢驗 IsaacVault 的安全性。

## 致謝
本專案的設計參考 [MSPasswordGenerator](https://github.com/Ayukawayen/MSPasswordGenerator) 的構想。
本專案藉助 [ISAAC](https://www.burtleburtle.net/bob/c/readable.c) 生成安全的密碼。
## MIT License
```
Copyright (c) 2024 bc6048

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
## 專案狀態
測試中

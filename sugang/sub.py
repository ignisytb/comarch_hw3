from selenium import webdriver
from selenium.webdriver.common.by import By
import schedule
import time


id="ignisytb"
pw="Taebin0408!"

def apply(sel):
    N=8

    for i in range(N):
        sel.execute_script("document.getElementById(\"1\").click();")
        time.sleep(0.2)
        sel.execute_script('document.getElementsByName("ApplyBtn")['+str(i)+'].click();')
        time.sleep(0.2)
        sel.execute_script('document.getElementById("btnApply").click();')
        time.sleep(0.2)
        sel.find_element(By.ID,"captchaInput").click()
        while True:
            frame=sel.find_element(By.ID,"captchaInput")
            val = frame.get_attribute("value")
            print("\r"+val,end='')
            if len(val)==5: break
        sel.execute_script("document.getElementById('btnCtAply').click();")
        time.sleep(0.2)
        alert = sel.switch_to.alert
        alert.accept()
        time.sleep(0.2)
        print()
    print("FINISH")




sel = webdriver.Chrome()

sel.get("https://sugang.dgist.ac.kr")
sel.execute_script("document.getElementById(\"username\").value='"+id+"';")
sel.execute_script("document.getElementById(\"passwordTest\").value='"+pw+"';")
sel.execute_script("document.getElementById(\"btnLogin\").click();")
time.sleep(1)
alert = sel.switch_to.alert
alert.accept()
time.sleep(1)
sel.execute_script("document.getElementById(\"headTab2\").click();")

schedule.every().day.at("10:00").do(apply,sel)
while True:
    schedule.run_pending()
    time.sleep(0.1)
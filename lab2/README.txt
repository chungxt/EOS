實驗環境說明

環境：linux ubuntu 16.04，GCC

使用：
	1.下載檔案 lab2_0660402.zip，並解壓縮得到一資料夾 lab2
	  可以暫時將 lab2 資料夾放在 /home/{user_name}/ 底下

	2.雖然 lab2 資料夾中的 run_lab2.sh 我已經事先change mode成可執行了
	  但如果不是可執行的話，就
	  >> chmod +x /home/{user_name}/lab2/run_lab2.sh

	3.由於我是將 run_lab2.sh 中寫成會執行位於 /lab2/preprocess 和 /lab2/lab2 兩個程式
	  因此將 /home/{user_name}/lab2/ 移到根目錄 / 底下
	  >>> sudo mv /home/{user_name}/lab2/ /
	
	*.程式主要分三個
	
	  LinkedList.c:	使用 linked list 實作 task priority queue 的主要結構，具有下列功能
					enqueue: 依 priority 排序並新增進 task queue
					dequeue: pop 出 priority 最高的 task
					print:	 印出所有 task 的資訊
					delete:  釋放 task queue 的記憶體
	  
	  preprocess.c: 亂數產生 15000 個 priority (1~15000) ，並寫入 data.txt
	  
	  lab2.c:		讀取 data.txt，enqueue 檔案中的 task
						將原先所有 task 的資訊寫入 result.txt
					enqueue 一個名稱為 "TEST TASK" 的 亂數 priority task，並計算enqueue時間
						將 enqueue "TEST TASK" 的所需時間寫入 result.txt
					dequeue priority 最高的 task
						將現在所有 task 的資訊寫入 result.txt
					最後使用 delete 釋放 task queue 的記憶體
					(其中註解裡面有 for input use 的功能)
		
	4.compile arm-base board
	  >> arm-unknown-linux-gnu-gcc -o preprocess preprocess.c
	  >> arm-unknown-linux-gnu-gcc -o lab2 lab2.c
	  move it to NFS dir

	5.because of no rc.d, apt-get, sudo, bash, use rc.local and sh
	  >> vi /etc/rc.d/rc.local
	  add 
	  >> sh ./lab2/run_lab2.sh 
	  and save

	6.reboot your board then it can run on boot


	*.雖然實驗板要用雖然要用arm base的gcc tool編譯
	  但是由於寫作業時手邊沒有實驗板，所以這邊我已經使用
	  >> gcc -o preprocess preprocess.c
	  >> gcc -o lab2 lab.c
	  編出兩個可以在linux的ubuntu上執行的 preprocess 和 lab2 兩隻程式
	
	*.由於要讓程式開機時執行，所以這邊需要
	  建立連結
	  >> sudo ln -s /lab2/run_lab2.sh /etc/init.d/run_lab2.sh
	  設定啟動順序
	  >> sudo update-rc.d -f run_lab2.sh defaults
	  之後關機之後再開機，程式就可以在 linux 上 run on boot 了
	  (我想這部分實驗板和電腦相同，全部唯一要改的部分應該就是編譯方式了)
	
	*.最後如果想要移除 run on boot，可以使用下列方式
	  >> sudo update-rc.d -f run_lab2.sh remove
	  如果移掉之後下次還是開機執行，就再重開機一次 (試過後基本上這樣就可以了)

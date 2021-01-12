luasql = require "luasql.mysql"



function max_trans_id()
	local sql = "SELECT * FROM requests WHERE trans_id IS NOT NULL"
	PrintDbgStr("[max_trans_id] "..sql)
	local cur = assert (con:execute(sql))
	local row = cur:fetch ({}, "a")
	local max_trans_id=0
	while row do 
		if  max_trans_id< tonumber(row.trans_id) then 
			max_trans_id= tonumber(row.trans_id)
		end
		row = cur:fetch (row, "a")
	end	
	return max_trans_id
end 

function main()
	PrintDbgStr("[main] lua started")
	env = assert (luasql.mysql())
	con = assert (env:connect("trading","xyz","passw0rd"))
	--cur = assert (con:execute("SELECT * FROM requests"))
	
	
	--cur = assert (con:execute("SELECT MAX(trans_id) FROM requests"))
	--PrintDbgStr(cur)
	--row = cur:fetch ({}, "a")
	--local last_id=0
	
	--PrintDbgStr ('Last id='..tostring(last_id))
	--PrintDbgStr ('Last trans_id='..tostring(last_trans_id))
	
	while true do
		

		local sql = string.format("SELECT * FROM requests WHERE cmd='new'")
		local cur = assert (con:execute(sql))
		local row = cur:fetch ({}, "a")
		
		
		while row do
			PrintDbgStr ("##################### new detected ###############")
			local trans_id
			local class_code
			if (row.trans_id ==nil) or (row.trans_id==0) then
				trans_id=max_trans_id()+1		-- если номера нет то присвавем максимальный+1
			else 
				trans_id=row.trans_id
			end 
			
			if (row.class_code ==nil) or (row.class_code=='spb') then 
				class_code="SPBXM"
			elseif (row.class_code =='usa') then 
				class_code="STOCK_USA"
			else 
				class_code=row.class_code
			end
			
			local new_transaction={  
				TRANS_ID   	= string.format("%d",trans_id),
				ACTION     	= "NEW_ORDER",
				CLASSCODE  	= class_code,
				SECCODE   	= row.sec_code,
				OPERATION   = row.oper,
				PRICE      	= tostring(row.price),
				QUANTITY   	= tostring(row.qty),
				ACCOUNT    	= "CLFINAMFA1",
				CLIENT_CODE	= "310933RI3C9M"
			}
			
			local sql =string.format("UPDATE requests SET cmd='new...' , status='...', trans_id=%d WHERE id = %d", trans_id, row.id ) -- ставим команду "new..." = ушло в обработку
			PrintDbgStr ("[main/new] "..sql)
			assert(con:execute(sql),"new/execute")
			assert(con:commit(),"new/commit")
			sendTransaction(new_transaction)
			row = cur:fetch (row, "a")
			sleep(500)
		end
		
		
		sql = string.format("SELECT * FROM requests WHERE cmd='kill'")
		cur = assert (con:execute(sql))
		row = cur:fetch ({}, "a")
		
		while row do
			PrintDbgStr ("##################### kill detected ###############")
			assert(row.order_num~=nil)
			if row.order_num~=nil then 
				local kill_transaction={  
					TRANS_ID   	= string.format("%d",row.trans_id),
					ORDER_KEY   = string.format("%d",row.order_num),
					ACTION     	= "KILL_ORDER",
					CLASSCODE  	= row.class_code,
					SECCODE   	= row.sec_code,
					ACCOUNT    	= "CLFINAMFA1",
					CLIENT_CODE	= "310933RI3C9M"
				}
				
				local sql =string.format("UPDATE requests SET cmd='kill...'  WHERE id = %d", row.id )
				PrintDbgStr("[main/kill]"..sql)
				assert(con:execute(sql),"kill/execute")
				assert(con:commit(),"kill/commit")
			
				sendTransaction(kill_transaction)
			end
			row = cur:fetch (row, "a")
			sleep(500)
		end

		sleep(500)
		----PrintDbgStr ('.')
		
	
	end;
end

function OnStop(flag)
	--PrintDbgStr ( "<OnStop>")
	assert(con:close(),"stop/con close")
	assert(env:close(),"stop/env close")

end


-- --PrintDbgStr кидает сообщение в DebugView
OnTransReplyCnt=0
function OnTransReply(trans_reply) -- tr=trans_reply
	OnTransReplyCnt=OnTransReplyCnt+1
	local t=trans_reply
	--PrintDbgStr ( "<OnTransReply>"..trans_reply.result_msg)
	local status
	local oper 
	if (trans_reply.error_code==0) then 
		status= "tr_ok"
	else
		status= "error"
	end 
	
	if (trans_reply.flags & (1<<17))==(1<<17) then 
		oper = 's'
	else 
		oper = 'b'	
	end
	
	
	local dt=trans_reply.date_time
	local datetime = 			string.format("%4d-%02d-%02d %02d:%02d:%02d.%03d",dt.year, dt.month, dt.day, dt.hour,dt.min,dt.sec,dt.ms)
	dt=trans_reply.gate_reply_time
	local gate_reply_time = 	string.format("%4d-%02d-%02d %02d:%02d:%02d.%03d",dt.year, dt.month, dt.day, dt.hour,dt.min,dt.sec,dt.ms)
	
	local sql= "SELECT * FROM requests WHERE trans_id = "..t.trans_id
	PrintDbgStr ("[OnTransReply] "..tostring(OnTransReplyCnt).." "..sql)
	local cur = assert (con:execute(sql))
	local row = cur:fetch ({}, "a")
	
	-- проверить что row<2
	if row ==nil then 
		--PrintDbgStr ( '<OnTransReply INSERT> ')
		sql=string.format("INSERT INTO requests ( error_code	, tr_status	,	order_num	, flags   	, trans_id 	, sec_code	,tr_datetime, gate_reply_time	, price		,qty		,class_code		, balance	,oper	, status	) \
										VALUES  ('%d'			, '%d'		,	'%d'		, '%d'  	, '%d'		, '%s'		, '%s'		, '%s'				, %f		,'%d'		,'%s'			, '%d'		,'%s'	, '%s'		)", 
												 t.error_code	,t.status	,	t.order_num , t.flags	, t.trans_id, t.sec_code, datetime	, gate_reply_time	, t.price	,t.quantity	,t.class_code  	, t.balance	,oper	, status	) 
		PrintDbgStr ("[OnTransReply] "..tostring(OnTransReplyCnt).." "..sql)
		assert(con:execute(sql),"")
		assert(con:commit())
		--sql="UPDATE requests SET result_msg='"..'----'.."' WHERE trans_id = "..t.trans_id 
		--cur = assert (con:execute(sql))
	else 
		--PrintDbgStr ( '<OnTransReply UPDATE>')
		----PrintDbgStr (tostring(t.error_code))
		----PrintDbgStr (tostring(t.tr_status ))
		----PrintDbgStr (tostring(t.order_num ))
		----PrintDbgStr (tostring(t.flags	))
		----PrintDbgStr (tostring(t.trans_id))
		----PrintDbgStr (tostring(t.sec_code))
		----PrintDbgStr (tostring(datetime	))
		----PrintDbgStr (tostring(gate_reply_time))
		----PrintDbgStr (tostring(t.price))
		----PrintDbgStr (tostring(t.quantity))
		----PrintDbgStr (tostring(t.class_code))
		----PrintDbgStr (tostring(t.balance))
		----PrintDbgStr (tostring(oper	))
		----PrintDbgStr (tostring(status))
		----PrintDbgStr (tostring(t.trans_id))
		if t.error_code==0 then 
			sql=string.format("UPDATE requests SET error_code=%d, tr_status=%d,	 order_num=%d,	flags=%d, trans_id=%d, sec_code='%s',tr_datetime='%s',gate_reply_time='%s',price=%f,qty=%d,	class_code='%s',balance=%d,oper='%s',status='%s' 	WHERE trans_id = %d",
												t.error_code, t.status ,	 t.order_num ,  t.flags	, t.trans_id, t.sec_code,   datetime	,   gate_reply_time	,    t.price, t.quantity, t.class_code, t.balance, oper	, status,				t.trans_id) 
		else 
			sql=string.format("UPDATE requests SET 	error_code=%d, 	tr_status=%d,	tr_datetime='%s',	gate_reply_time='%s',status='%s' WHERE trans_id = %d",
													t.error_code, 	t.status ,	 	datetime	,   	gate_reply_time	, 	status,				t.trans_id)
		end
			
		--PrintDbgStr ( '<<<')
		--PrintDbgStr(tostring(t.trans_id))
		--PrintDbgStr(tostring(row.trans_id))
		--PrintDbgStr ( '>>>')
		--assert(t.trans_id==row.trans_id)
		--PrintDbgStr ( sql)
		PrintDbgStr ("[OnTransReply] "..tostring(OnTransReplyCnt).." "..sql)
		assert(con:execute(sql))						
		assert(con:commit())
	
	end
	OnTransReplyCnt=OnTransReplyCnt-1	
end 

function OnTrade(trade)
	--PrintDbgStr ( "<OnTrade>")
	--trd = assert (con:execute("SELECT * FROM trades"))
	
end 
OnOrderCnt=0
function OnOrder(order)
	OnOrderCnt=OnOrderCnt+1
	local o=order
	local status
	local oper
	--PrintDbgStr ( "<OnOrder>")
	--PrintDbgStr ( "  "..order2str(order))
	
		
	----PrintDbgStr ( "<Activayteton time     >"..tostring(o.activation_time))
	----PrintDbgStr ( "<ext_order_status time >"..tostring(o.ext_order_status))
	
	if (order.flags&1==1) then 
		status= "active"
	else
		if (order.flags&2)==2 then 
			status= "canceled"
		else 
			status= "completed"
		end
	end 
	
	if (order.flags & 4)==4 then 
		oper = 'S'
	else 
		oper = 'B'	
	end
	
	local dt=order.datetime
	local datetime 		 	= string.format("%4d-%02d-%02d %02d:%02d:%02d.%03d",dt.year, dt.month, dt.day, dt.hour,dt.min,dt.sec,dt.ms)
	dt=order.withdraw_datetime
	local withdraw_datetime = string.format("%4d-%02d-%02d %02d:%02d:%02d.%03d",dt.year, dt.month, dt.day, dt.hour,dt.min,dt.sec,dt.ms)
	
	
	local sql= "SELECT order_num FROM requests WHERE order_num = "..o.order_num
	PrintDbgStr ("[OnOrder] "..tostring(OnOrderCnt).." "..sql)
	local cur = assert (con:execute(sql))
	local row = cur:fetch ({}, "a")
	if row ==nil then 
		--PrintDbgStr ('[on ORDER INSERT]'..tostring(o.order_num))
		sql=string.format("INSERT INTO requests (order_num	, flags   	, trans_id 	, sec_code	, datetime	, withdraw_datetime	, price		,qty	,class_code		, balance	,oper	, status) 										VALUES  ('%d'		, '%d'  	, '%d'		, '%s'		, '%s'		, '%s'				, %f		,'%d'	,'%s'			, '%d'		,'%s'	, '%s')", 
												o.order_num , o.flags	, o.trans_id, o.sec_code, datetime	, withdraw_datetime	, o.price	,o.qty	,o.class_code  	, o.balance	,oper	, status) 
		PrintDbgStr ("[OnOrder] "..tostring(OnOrderCnt).." "..sql)
		assert(con:execute(sql))
		assert(con:commit())
		--PrintDbgStr  (sql)
	else 
		--PrintDbgStr ('[on ORDER UPDATE]'..tostring(o.order_num))
		sql=string.format("UPDATE requests SET order_num=%d,flags=%d, sec_code='%s',datetime='%s',withdraw_datetime='%s',price=%f,qty=%d,class_code='%s',balance=%d,oper='%s',status='%s' WHERE order_num=%d",
												o.order_num , o.flags, o.sec_code, datetime	, withdraw_datetime	, o.price	,o.qty	,o.class_code  	, o.balance	,oper	, status,				o.order_num) 
												--, o.trans_id removed becase dup onOrder with =0
		PrintDbgStr ("[OnOrder] "..tostring(OnOrderCnt).." "..sql)
		assert (con:execute(sql))												
		assert(con:commit())
		--PrintDbgStr  (sql)
	end
	OnOrderCnt=OnOrderCnt-1
end 

function OnQuote_(class, sec )
	
	local ql2 = getQuoteLevel2(class, sec)
	-- --PrintDbgStr("<OnQuote> ".. class .." ".. sec .." "..ql2.bid_count.." "..ql2.offer_count)
	
 
	-- bid_count 	STRING Количество котировок покупки. При отсутствии спроса возвращается значение «0» 
	-- offer_count 	STRING Количество котировок продажи. При отсутствии предложения возвращается значение «0» 
	-- bid 			TABLE Котировки спроса (покупки). При отсутствии спроса возвращается пустая строка 
	-- offer 		TABLE Котировки предложений (продажи). При отсутствии предложения возвращается пустая строка 

	--if class ~=myFloat.classcode or sec ~= myFloat.seccode then
	--	return 
	--end
	----PrintDbgStr(ql2.bid_count.." "..tostring(ask_count))
	
	local bid_count = tonumber(ql2.bid_count)	
	local ask_count = tonumber(ql2.offer_count)
	
	--ask1=ql2.offer[2].price
	--ask0=ql2.offer[1].price
	--bid0=ql2.bid[bid_count  ].price
	--bid1=ql2.bid[bid_count-1].price
	----PrintDbgStr("<OnQuote> ".. class .." ".. sec .." "..bid1.." "..bid0..":"..ask0.." "..ask1)
	----PrintDbgStr(tostring(bid_count).." "..tostring(ask_count))
	
	--depth=20
	--if true then 
	--	--PrintDbgStr("====================")
	--	-- for i = ask_count,1,-1 do 
	--	for i = math.min(depth,ask_count),1,-1 do 
	--		--PrintDbgStr("ask "..ql2.offer[i].price .." "..ql2.offer[i].quantity)
	--	end	
	--	--PrintDbgStr("[       GAP         ]")
	--	--for i = bid_count,1,-1 do 
	--	for i = bid_count,bid_count-math.min(bid_count,depth)+1,-1 do 
	--		--PrintDbgStr("bid "..ql2.bid[i].price .." "..ql2.bid[i].quantity)
	--	end
	--	--PrintDbgStr("====================")
	--end
	
end
function OnStop()
  --PrintDbgStr ( "<OnStop>")
end;
function OnClose()
  --PrintDbgStr ( "<OnClose>")
end;
function OnInit(path )
  --PrintDbgStr ( "<OnInit> "..path)
end;

myFloat={
	trans_id= "0",
	order_key = "0",
	price = "10",
	--quantity ="1",
	--operation = "B",
	status = "active",
	seccode = "AFLT",
	classcode ="TQBR"
}

transaction = {
	ACCOUNT="L01+00000F00",
	CLIENT_CODE="7655m65",
	TYPE="M",
	TRANS_ID="7",
	CLASSCODE="STOCK_USA",
	SECCODE="WFC.US",
	ACTION="NEW_ORDER",
	OPERATION="B",
	PRICE="29.7",
	QUANTITY="1"
}



myTransaction={
      ["TRANS_ID"]   = "777",
      ["ACTION"]     = "NEW_ORDER",
      ["CLASSCODE"]  = "TQBR",
	  ["SECCODE"]    = "AFLT",
      ["ACCOUNT"]    = "L01+00000F00",  --  вам запрещена работа по данному счтеу 
      ["PRICE"]      = "75.30",
      ["OPERATION"]  = "B", -- покупка (BUY)
      ["QUANTITY"]   = "1", -- количество
	  ["CLIENT_CODE"]= "7655m65"
}
spbTransaction={  -- Работает 
      ["TRANS_ID"]   = "37",
      ["ACTION"]     = "NEW_ORDER",
      ["CLASSCODE"]  = "SPBXM",
	  ["SECCODE"]    = "AAPL.SPB",
      ["ACCOUNT"]    = "CLFINAMFA1",  --  OK!
      ["PRICE"]      = "36",
      ["OPERATION"]  = "B", -- покупка (BUY)
      --["TYPE"]       = "M", -- по рынку (MARKET)
      ["QUANTITY"]   = "1", -- количество
	  ["CLIENT_CODE"]= "310933RI3C9M",
	  --["CLIENT_CODE"]= "7618rzy", -- ИИС -  7618rzy - срочные фьючерсы
      --["PRICE"]      = "76",
      ["COMMENT"]    = "Покупка фьючерсов скриптом"
   }
spbTransaction2={  -- Работает 
      TRANS_ID   = "37",
      ACTION     = "NEW_ORDER",
      CLASSCODE  = "SPBXM",
	  SECCODE    = "AAPL.SPB",
      ACCOUNT    = "CLFINAMFA1",  --  OK!
      PRICE      = "36",
      OPERATION  = "B", -- покупка (BUY)
      QUANTITY   = "1", -- количество
	  CLIENT_CODE= "310933RI3C9M",
      COMMENT    = "Покупка фьючерсов скриптом"
   }
   
usTransaction={         -- Работает 
      ["TRANS_ID"]   = "113",
      ["ACTION"]     = "NEW_ORDER",
	  ["CLASSCODE"]  = "STOCK_USA",
      --["SECCODE"]    = "AAPL.US",
      --["PRICE"]      = "133",
	  
	  ["SECCODE"]    = "WFC.US",
      ["PRICE"]      = "29",
      
      ["ACCOUNT"]    = "MCU1100",  --  OK!
      ["OPERATION"]  = "B", -- покупка (BUY)
      --["TYPE"]       = "M", -- по рынку (MARKET)
      ["QUANTITY"]   = "1", -- количество
	  ["CLIENT_CODE"]= "7655m65",
      ["COMMENT"]    = "USA"
   }
 
CancelOrder={
      ["TRANS_ID"]   = "114",
      ["ACTION"]     = "KILL_ORDER",
	  -- CLASSCODE=TQBR; SECCODE=RU0009024277; TRANS_ID=5; ACTION=KILL_ORDER; 
	  ["ORDER_KEY"]  ="493567154892000",
      --["CLASSCODE"]  = "SPFEQ",
      
	  --["CLASSCODE"]  = "STOCK_USA",
      ---["SECCODE"]    = "AAPL.US",
      --["ACCOUNT"]    = "L01+00000F00",  --  вам запрещена работа по данному счтеу 
      --["PRICE"]      = "370",
      
      ["CLASSCODE"]  = "TQBR",
	  --["SECCODE"]    = "AFLT",
      --["ACCOUNT"]    = "L01+00000F00",  --  вам запрещена работа по данному счтеу 
      --["PRICE"]      = "75",
      
      -- ["OPERATION"]  = "B", -- покупка (BUY)
      --["TYPE"]       = "M", -- по рынку (MARKET)
      --["QUANTITY"]   = "1", -- количество
      -- ["ACCOUNT"]    = "FZQU119294A", [212704] Result: Указанный счет депо не найден: "FZQU119294A"
      -- ["ACCOUNT"]    = "310933R2M9J",  -- [212704] Result: Указанный счет депо не найден: "310933R2M9J"
      --["ACCOUNT"]    = "КлФ-916085",
	  --["CLIENT_CODE"]= "7655m65",
	  ["CLIENT_CODE"]= "7655m65",
      --["PRICE"]      = "76",
      --["COMMENT"]    = "Покупка фьючерсов скриптом"
   }
   

function order2str(order)
	dt    = order.datetime
	dtoff = order.withdraw_datetime
	order_num= order.order_num
	str = "order["..tostring(order_num).."] ".. order.sec_code .. " flags:"..tostring(order.flags) .." status_ext:"..order.ext_order_status .. " trans_id:".. order.trans_id ; 
	str = str .. " time on: " ..tostring(dt.day).." "..tostring(dt.hour)   ..":"..tostring(dt.min)   ..":"..tostring(dt.sec);
	str = str .. " withdraw: "..tostring(dtoff.day).." "..tostring(dtoff.hour)..":"..tostring(dtoff.min)..":"..tostring(dtoff.sec);
	flags = order.flags
	if (flags&1==1) then 
		str = str .. " active";
	else
		if (flags&2) then 
			str = str .." canceled";
		else 
			str = str .. " completed";
		end
	end 

	return str 
end

function cancelAllOrders()

	num= getNumberOf ("orders")
	--PrintDbgStr("<cancelAllOrders> num orders:"..tostring(num).." ---------------------------"); 
	num = getNumberOf("orders")
	for i = 0,num do
		order= getItem("orders",i)
		--PrintDbgStr(tostring(i).." "..order2str(order))
		if (order.flags&1==1) then
			--PrintDbgStr(tostring(i).." "..tostring(order.order_num).."  cancaling..."..order.client_code.." "..order.sec_code.." ".. order.class_code)
			myFloat. status ="active"
			cancelOrder(order.order_num, order.client_code,  order.sec_code, order.class_code  )
			while myFloat.status=="active" do
				sleep(100)
			end
			myFloat.status="active"
			--PrintDbgStr(tostring(i).." "..tostring(order.order_num).."  canceled")
			
		end 
   end;
end;


function cancelOrder(order_num,clientCode,secCode, classCode)
	--PrintDbgStr("<cancelOrder> "..tostring(order_num)); 
	local order={
		["TRANS_ID"]=tostring(110),
		["ACTION"]     = "KILL_ORDER",
		["CLASSCODE"]=classCode,
		["ORDER_KEY"]  = tostring(order_num),
		["SECCODE"]=secCode,
      --["CLASSCODE"]  = "TQBR",
		["CLIENT_CODE"]= clientCode,
     }
	 
	     
         		 
	res = sendTransaction(order)
	--PrintDbgStr("<CancelOrder>"..tostring(res))
end;

bid0 = ""
bid1 = ""
ask0 = ""
ask1 = ""


   
function main1() 
	while true do
	
	
	end;
	
	
	cancelAllOrders()
	--res=sendTransaction(usTransaction)
	--res=sendTransaction(spbTransaction)
	--PrintDbgStr("Send result="..tostring(res))
	
end;    
function main_()

	res=Subscribe_Level_II_Quotes("TQBR","AFLT")
	res=Subscribe_Level_II_Quotes("STOCK_USA","TSLA.US")
	--PrintDbgStr("<Subscribe_Level_II_Quotes>"..tostring(res))
	if res==false then
		message("Error: Subscribe_Level_II_Quotes")
		return -1
	end
	num= getNumberOf ("orders")
	--PrintDbgStr("num orders:"..tostring(num))
	--sendTransaction(spbTransaction)
	cancelAllOrders()

	--sleep(10000)
	
	--while true do
	--	sleep(1000)
	--end
	--message(os.date()) --раз в секунду выводит текущие дату и время		
	list = getClassesList ()
	--PrintDbgStr("classes: "..list)
	state=isConnected ()	
	--PrintDbgStr("connected: "..state)
	res= getScriptPath()
	--PrintDbgStr("script: "..res)
	-- message (state)
	-- message (res,"3")
	res = getInfoParam ("VERSION")
	--PrintDbgStr ("version: "..res)
	-- list = getClassInfo ("TQBR")
	-- list = getClassInfo ("STOCK_USA")
	-- --PrintDbgStr ( list)
	sec_list = getClassSecurities("TQBR") 
	--sec_list = getClassSecurities("STOCK_USA") 
	----PrintDbgStr ( sec_list)
	
	while bid1=="" do
		sleep(100)
	end
	myFloat.price = bid1
	myFloat.status= "transaction"
	myTransaction["PRICE"]=myFloat.price
	--PrintDbgStr("<sendTransaction 0>")
	
	while true do
		-- watching quotes....
		sleep(500)
	end 
	
	sendTransaction(myTransaction)
	while myFloat.status =="transaction" do
		sleep(100)
	end
				
				
				
	counter =0 
	while true do
		--message(os.date()) --раз в секунду выводит текущие дату и время
		counter = counter+1
		----PrintDbgStr("-------- "..tostring(counter))
		sleep(500)
		--if (counter == 2) then 
			--message("FUCK")
			--sendTransaction(Transaction)
			-- sendTransaction(CancelOrder)
		--end
		-- if tonumber(bid1) != tonumber(myFloat.price) then 
		if bid1 ~= myFloat.price then 
			--PrintDbgStr("!!!!!!! change bid  "..bid1)
			cancelOrder(myFloat.order_num)
			while myFloat.status=="active" do
				sleep(500)
			end
			if myFloat.status=="completed" then
				message("FUCK - completed")
				return 
			end
			if myFloat.status=="canceled" then 
				myFloat.price = bid1
				myFloat.status= "transaction"
				myTransaction["PRICE"]=myFloat.price
				myTransaction["TRANS_ID"]=tostring(counter)
				--PrintDbgStr("<sendTransaction>")
				sendTransaction(myTransaction)
				while myFloat.status =="transaction" do
					sleep(500)
				end
			end
		end
		
	end 
	

   --PrintDbgStr("Trans:"..Transaction["TRANS_ID"])
   local Result = sendTransaction(usTransaction);
   --PrintDbgStr("Result:"..Result)
	-- tbl =  getPortfolioInfo (STRING firm_id, STRING client_code)
	-- --PrintDbgStr ( "end")
	tbl = getPortfolioInfo("MC0061900000","310933R2M9J")
	--tbl = AllocTable()
	-- tbl = getNumberOf("client_codes")
	 --PrintDbgStr ( tbl.in_assets)
	 --PrintDbgStr ( tbl.val_short)
	 --PrintDbgStr ( tbl.val_long)
	 --PrintDbgStr ( tbl.assets)
	 
	 -- q= getQuoteLevel2("TQBR","TSLA")
	 q= getQuoteLevel2("STOCK_USA","T1SLA")
	 --PrintDbgStr ( q.bid_count)
	 --PrintDbgStr ( q.offer_count)
	 --PrintDbgStr ( q.bid)
	 --PrintDbgStr ( q.offer)
	 
	 --end

	-- message (tbl.in_assets)
end ;


function OnConnected( flag)
	 --PrintDbgStr ( "OnConnected")
	

end ;


function OnDisconnected( )
	 --PrintDbgStr ( "OnDisConnected")
	

end ;




-- [212704] classes:CROSSRATE,EQRP_INFO,INDX,RTSIDX,USDRUB,EQOB,EQEO,EQDB,SMAL,TQBR,TQDE,TQOB,TQIF,TQTF,TQTD,TQOD,TQTE,TQCB,CETS,INDXC,SPBFUT,
-- FUTSPREAD,STOCK_USA,TQIR,TQIU,TQPI,TQOE,

-- 	[212704] A.US,ACOR.US,ACIW.US,AC.US,AA.US,AAL.US,AADR.US,AAOI.US,AAT.US,AAN.US,ABUS.US,AAPL.US,AAON.US,AAME.US,AAWW.US,AAP.US,ABMD.US,ACRX.US,ABBV.US,AAXJ.US,AB.US,AAXN.US,ABEV.US,ABT.US,ABC.US,ABCB.US,ACGL.US,ABEt.US,ACH.US,ABRpC.US,ABM.US,ABG.US,ABRpA.US,ACBI.US,ABTX.US,ACB.US,ACA.US,ABR.US,ACHC.US,ACAD.US,ACC.US,ACIA.US,ACCt.US,ACLS.US,ACM.US,ACN.US,ACRE.US,ACU.US,ACV.US,ACWF.US,AEL.US,ACWI.US,ACNB.US,ADES.US,ADBE.US,ADCT.US,ADC.US,ACTG.US,ADM.US,ADMS.US,ADMA.US,ADNT.US,ADPT.US,AFI.US,ACY.US,AFB.US,ADP.US,AEP.US,AEY.US,ADT.US,ADSW.US,ACWX.US,ADI.US,ADRt.US,AEGN.US,ADS.US,ADX.US,AE.US,AEHR.US,ADXS.US,AFT.US,ADSK.US,ADUS.US,AEIS.US,AERI.US,AEB.US,AEE.US,AFK.US,AEt.US,AEM.US,AFG.US,AES.US,AER.US,AG.US,AGCt.US,AGGY.US,AGI.US,AGRt.US,AFL.US,AHH.US,AGG.US,AIG.US,AGM.US,AIV.US,AI.US,AGt.US,AGQ.US,AGR.US,AGMpA.US,AGS.US,AIM.US,AGX.US,AGZ.US,AIR.US,AL.US,AHT.US,AHTpD.US,AHLpC.US,AIRI.US,AIF.US,AJG.US,AIT.US,AIN.US,AIZ.US,AJRD.US,AKR.US,ALB.US,ALLpB.US,ALE.US,ALEX.US,ALG.US,ALX.US,ALK.US,ALLE.US,ALL.US,AMN.US,AMPE.US,ALLY.US,AMH.US,ALLYpA.US,AMBC.US,ALSN.US,AM.US,ALV.US,AMC.US,AME.US,AMG.US,AMP.US,AMRC.US,AMS.US,AMT.US,AP.US,AMU.US,AMX.US,AMZA.US,AN.US,ANET.US,ANF.US,ANH.US,ANHpC.US,APH.US,AOA.US,AOK.US,APAM.US,APD.US,ANHpA.US,AOR.US,AON.US,AOM.US,AOS.US,ANTM.US,APLE.US,ADTN.US,AFMD.US,AIMT.US,AKER.US,AGLE.US,AGIt.US,AGYS.US,AKAM.US,AGNC.US,AGZD.US,AIRR.US,AGTC.US,AGRX.US,AIMC.US,ALCt.US,AIA.US,AKCA.US,AIRT.US,AKAOQ.US,AKBA.US,AKRXQ.US,AKTS.US,ALDX.US,ALGN.US,ALTY.US,ALNY.US,ALGT.US,ALIM.US,ALXN.US,ALOT.US,ALKS.US,ALLK.US,ALLt.US,ALLT.US,AMAG.US,ALNA.US,ALRN.US,ALRM.US,AMGN.US,AMBA.US,ALTR.US,ALSK.US,AMKR.US,AMAT.US,AMOT.US,AMCX.US,AMD.US,AMNB.US,AMEH.US,AMED.US,AMPH.US,AMRB.US,APA.US,ANIK.US,AMSF.US,AMSC.US,AMTD.US,AMTX.US,AMSWA.US,AMWD.US,ANDE.US,ANIP.US,ANAT.US,ANPC.US,ANGI.US,ANGL.US,ANSS.US,ANGt.US,AMZN.US,ANAB.US,APDN.US,AOSL.US,APHA.US,APEI.US,APLS.US,APt.US,APTV.US,APPF.US,APPN.US,ARDX.US,APOG.US,APTS.US,APPS.US,AQUA.US,AR.US,ARA.US,APRN.US,ARCT.US,ARCt.US,ARCH.US,ARCC.US,ARAY.US,ARCB.US,ARDC.US,ARC.US,ARD.US,ARE.US,ARI


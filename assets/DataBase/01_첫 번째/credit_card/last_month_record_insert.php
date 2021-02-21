<html>
<title>last_month_record_insert</title>
<body>

<H1>last_month_record_insert</H1>

<form method="post" action="last_month_record_DB_insert.php">
	<label for="credit_card_name">신용카드 이름</label>
	<input type="text" id="credit_card_name" name="credit_card_name"/><br/>

	<label for="month">월</label>
	<input type="text" id="month" name="month"/><br/>

	<label for="month_record">월에 해당하는 실적 금액</label>
	<input type="text" id="month_record" name="month_record"/><br/>

	<input type="submit" value="전월 실적 추가" name="submit"/>

</form><br/>

<form method="post" action="main_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>
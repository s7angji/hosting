<html>
<title>apply_month_insert</title>
<body>

<H1>apply_month_insert</H1>

<form method="post" action="apply_month_DB_insert.php">

	<label for="month">월</label>
	<input type="text" id="month" name="month"/><br/>

	<input type="submit" value="적용할 월(month) 변경" name="submit"/>

</form><br/>

<form method="post" action="main_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>
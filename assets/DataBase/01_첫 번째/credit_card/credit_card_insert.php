<html>
<title>credit_card_insert</title>
<body>

<H1>credit_card_insert</H1>

<form method="post" action="credit_card_DB_insert.php">
	<label for="credit_card_name">신용카드 이름</label>
	<input type="text" id="credit_card_name" name="credit_card_name"/><br/>

	<label for="phone_num">고객센터 전화번호</label>
	<input type="text" id="phone_num" name="phone_num"/><br/>

	<label for="web">web 페이지 주소</label>
	<input type="text" id="web" name="web"/><br/>

	<label for="rating">회원 등급</label>
	<input type="text" id="rating" name="rating"/><br/><br/>

	<input type="submit" value="신용카드 추가" name="submit"/>

</form><br/>

<form method="post" action="main_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>
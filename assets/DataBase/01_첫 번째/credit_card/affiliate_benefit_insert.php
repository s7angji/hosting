<html>
<title>affiliate_benefit_insert</title>
<body>

<H1>affiliate_benefit_insert</H1>

<b>할인 그룹 선택</b><br/>

<form method="post" action="affiliate_benefit_DB_insert.php">

	<label for="credit_card_name">신용카드 이름</label>
	<input type="text" id="credit_card_name" name="credit_card_name"/><br/>

	<label for="month_condition">전월 실적 조건</label>
	<input type="text" id="month_condition" name="month_condition"/><br/>

	<label for="aff_name">가맹점 이름</label>
	<input type="text" id="aff_name" name="aff_name"/><br/>

	<label for="discount_rate">혜택 내용(할인율)</label>
	<input type="text" id="discount_rate" name="discount_rate"/><br/><br/>

	<input type="submit" value="가맹점 추가" name="submit"/>

</form><br/>

<form method="post" action="main_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>
<html>
<title>discount_group_insert</title>
<body>

<H1>discount_group_insert</H1>

<b>할인 그룹 선택</b>

<form method="post" action="discount_group_DB_insert.php">
	<label for="쇼핑">쇼핑</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="쇼핑"/><br/>

	<label for="주유">주유</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="주유"/><br/>

	<label for="교통">교통</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="교통"/><br/>

	<label for="모바일">모바일</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="모바일"/><br/>

	<label for="뷰티">뷰티</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="뷰티"/><br/>

	<label for="병원">병원</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="병원"/><br/>

	<label for="약국">약국</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="약국"/><br/>

	<label for="학원">학원</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="학원"/><br/>

	<label for="인터넷">인터넷</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="인터넷"/><br/>

	<label for="엔터테인먼트">엔터테인먼트</label>
	<input type="radio" id="discount_group_name" name="discount_group_name" value="엔터테인먼트"/><br/><br/>

	<label for="aff_name">가맹점 이름</label>
	<input type="text" id="aff_name" name="aff_name"/><br/>

	<input type="submit" value="새로운 가맹점의 할인 그룹 지정" name="submit"/>

</form><br/>

<form method="post" action="main_insert.php">
	<input type="submit" value="이전으로 ..." name="submit"/><br/>
</form>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>
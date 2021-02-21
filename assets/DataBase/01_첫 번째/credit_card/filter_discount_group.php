<html>
<title>filter_discount_group</title>
<body>

<H1>filter_discount_group</H1>

<b>할인 그룹 선택</b>

<form method="post" action="filter_discount_group_DB.php">
	<label for="쇼핑">쇼핑</label>
	<input type="checkbox" id="shopping" name="shopping" value="쇼핑"/><br/>

	<label for="주유">주유</label>
	<input type="checkbox" id="refuel" name="refuel" value="주유"/><br/>

	<label for="교통">교통</label>
	<input type="checkbox" id="traffic" name="traffic" value="교통"/><br/>

	<label for="모바일">모바일</label>
	<input type="checkbox" id="mobile" name="mobile" value="모바일"/><br/>

	<label for="뷰티">뷰티</label>
	<input type="checkbox" id="beauty" name="beauty" value="뷰티"/><br/>

	<label for="병원">병원</label>
	<input type="checkbox" id="hospital" name="hospital" value="병원"/><br/>

	<label for="약국">약국</label>
	<input type="checkbox" id="pharmacy" name="pharmacy" value="약국"/><br/>

	<label for="학원">학원</label>
	<input type="checkbox" id="edu" name="edu" value="학원"/><br/>

	<label for="인터넷">인터넷</label>
	<input type="checkbox" id="internet" name="internet" value="인터넷"/><br/>

	<label for="엔터테인먼트">엔터테인먼트</label>
	<input type="checkbox" id="entertainment" name="entertainment" value="엔터테인먼트"/><br/><br/>

	<input type="submit" value="할인 그룹 필터하여 할인 혜택 골라 보기" name="submit"/>

</form><br/>

<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>
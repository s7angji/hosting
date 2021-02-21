<html>
<head>
<title>filter_credit_card</title>
</head>
<body>

<H1>filter_credit_card</H1>

<b>원하지 않는 신용카드는 Text필드를 비우고 진행하세요!!!</b><br/><br/>

<?php
	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
		or die('Error Connecting to MySQL server.');

	$query = "select * from credit_card";
	$result = mysqli_query($dbc, $query)
		or die('Error Querying database.');

	$filter = array("", "");
	$i = 0;
	while($row = mysqli_fetch_array($result)){
		$filter[$i] = $row['credit_card_name'];
		$i++;
	}
	mysqli_close($dbc);
?>

<form method="post" action="filter_all_DB.php">
<?php
	echo '<label for="num"> 등록된 신용카드의 총 갯수 </label>';
	echo '<input type="text" id="num" name="num" value="' . $i . '"/><br/>';

	for($c = 0; $c < $i; $c++){
		echo '<label for="filter' . $c . '">' . $filter[$c] . '</label>';
		echo '<input type="text" id="filter' . $c . '" name="filter' . $c . '" value="' . $filter[$c] . '"/><br/>';
	}
?>

<br/><br/><b>할인 그룹 선택!!!</b><br/><br/>

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

	<input type="submit" value="신용 카드 & 할인 그룹 필터" name="submit"/>

</form><br/>


<form method="post" action="main.php">
	<input type="submit" value="main 화면으로 ..." name="submit"/><br/>
</form>

</body>
</html>
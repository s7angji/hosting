<html>
<head>
<title>main_credit_card_information</title>
</head>
<body>

<?php
	$dbc = mysqli_connect('127.0.0.1', 'root', 'apmsetup', 'credit_card')
		or die('Error Connecting to MySQL server.');
	$query = "select * from credit_card";
	$result = mysqli_query($dbc, $query)
		or die('Error Querying database.');
	echo '<b> �ſ�ī���̸� ��������ȭ��ȣ web�������ּ� ȸ�����</b><br/>';
	while($row = mysqli_fetch_array($result)){
		echo $row['credit_card_name'] . ' (' . $row['phone_num'] . ') ' . $row['web'] .
			' ' . $row['rating'] . '<br/>';
	}
	mysqli_close($dbc);
?>
</body>
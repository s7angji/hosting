<html>
<head>
<title>filter_credit_card</title>
</head>
<body>

<H1>filter_credit_card</H1>

<b>������ �ʴ� �ſ�ī��� Text�ʵ带 ���� �����ϼ���!!!</b><br/><br/>

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
	echo '<label for="num"> ��ϵ� �ſ�ī���� �� ���� </label>';
	echo '<input type="text" id="num" name="num" value="' . $i . '"/><br/>';

	for($c = 0; $c < $i; $c++){
		echo '<label for="filter' . $c . '">' . $filter[$c] . '</label>';
		echo '<input type="text" id="filter' . $c . '" name="filter' . $c . '" value="' . $filter[$c] . '"/><br/>';
	}
?>

<br/><br/><b>���� �׷� ����!!!</b><br/><br/>

	<label for="����">����</label>
	<input type="checkbox" id="shopping" name="shopping" value="����"/><br/>

	<label for="����">����</label>
	<input type="checkbox" id="refuel" name="refuel" value="����"/><br/>

	<label for="����">����</label>
	<input type="checkbox" id="traffic" name="traffic" value="����"/><br/>

	<label for="�����">�����</label>
	<input type="checkbox" id="mobile" name="mobile" value="�����"/><br/>

	<label for="��Ƽ">��Ƽ</label>
	<input type="checkbox" id="beauty" name="beauty" value="��Ƽ"/><br/>

	<label for="����">����</label>
	<input type="checkbox" id="hospital" name="hospital" value="����"/><br/>

	<label for="�౹">�౹</label>
	<input type="checkbox" id="pharmacy" name="pharmacy" value="�౹"/><br/>

	<label for="�п�">�п�</label>
	<input type="checkbox" id="edu" name="edu" value="�п�"/><br/>

	<label for="���ͳ�">���ͳ�</label>
	<input type="checkbox" id="internet" name="internet" value="���ͳ�"/><br/>

	<label for="�������θ�Ʈ">�������θ�Ʈ</label>
	<input type="checkbox" id="entertainment" name="entertainment" value="�������θ�Ʈ"/><br/><br/>

	<input type="submit" value="�ſ� ī�� & ���� �׷� ����" name="submit"/>

</form><br/>


<form method="post" action="main.php">
	<input type="submit" value="main ȭ������ ..." name="submit"/><br/>
</form>

</body>
</html>
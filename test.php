<html>
<body>
<?php 

echo "GET variables: "; var_dump($_GET); echo "<br>";
echo "POST variables: "; var_dump($_POST); echo "<br>";

$servername = "localhost";
$username = "caleb";
$password = "Panic!AtTheDisco1";
$dbname = "test_db";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}
echo "Connected successfully<br>";

$sql = "SELECT * FROM users";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // output data of each row
    while($row = $result->fetch_assoc()) {
        echo "uid: " . $row["uid"] . "<br>";
        echo "Username: " . $row["username"] . "<br>";
        echo "Password: " . $row["password"] . "<br>";
        echo "Name: " . $row["first_name"];
    }
} else {
    echo "0 results";
}

$conn->close();

?>

<br><br>

<form action="thing.php" method="post">
    Name: <input type="text" name="name" /></br>
    Fork: <input type="password" name="fork" /><br>
    <input type="submit" value="Submit this" />
</form>

<?php phpinfo(); ?>
</body>
</html>
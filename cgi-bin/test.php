<?php
echo "<html><body>";
echo "<h1>PHP CGI Test</h1>";
echo "<p>Current time: " . date('Y-m-d H:i:s') . "</p>";
echo "<h2>Server Variables:</h2>";
echo "<pre>";
foreach ($_SERVER as $key => $value) {
    echo htmlspecialchars($key) . " = " . htmlspecialchars($value) . "\n";
}
echo "</pre>";
echo "<h2>Environment Variables:</h2>";
echo "<pre>";
$env_vars = ['PATH', 'QUERY_STRING', 'REQUEST_METHOD', 'CONTENT_TYPE', 'CONTENT_LENGTH'];
foreach ($env_vars as $var) {
    $value = getenv($var);
    if ($value !== false) {
        echo htmlspecialchars($var) . " = " . htmlspecialchars($value) . "\n";
    }
}
echo "</pre>";
echo "</body></html>";
?>

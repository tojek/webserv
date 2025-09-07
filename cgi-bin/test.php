<?php
echo "<html><body>";
echo "<h1>PHP CGI Test</h1>";
echo "<p>Current time: " . date('Y-m-d H:i:s') . "</p>";
echo "<h2>Environment Variables:</h2>";
echo "<pre>";
foreach ($_ENV as $key => $value) {
    echo htmlspecialchars($key) . " = " . htmlspecialchars($value) . "\n";
}
echo "</pre>";
echo "</body></html>";
?>

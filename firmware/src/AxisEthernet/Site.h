#pragma once

const char* html_page = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AJAX Request Every 500ms</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
        }
        #number {
            font-size: 2em;
            font-weight: bold;
        }
        #status {
            color: gray;
        }
    </style>
</head>
<body>
    <div id="number">Waiting for number...</div>

    <script>
        // Function to send AJAX request
        function sendAjaxRequest() {
            const xhr = new XMLHttpRequest();
            xhr.open('GET', '/get-number', true);  // Backend endpoint here
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        // Parse the response and update the page
                        const response = parseInt(xhr.responseText, 10);
                        if (!isNaN(response)) {
                            document.getElementById('number').textContent = response;
                        } else {
                            document.getElementById('status').textContent = 'Received non-integer response!';
                        }
                    } else {
                        document.getElementById('status').textContent = 'Error: ' + xhr.status;
                    }
                }
            };
            xhr.send();
        }


        // Send AJAX requests every 500ms
        setInterval(sendAjaxRequest, 50);

    </script>
</body>
</html>
)rawliteral";

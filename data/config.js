const config_api_url = "http://192.168.1.56/api/config";

let rzConfig =
    [
        {
            "id": "wifi",
            "title": "Network Configuration",
            "parameters": [
                {
                    "title": "Wifi",
                    "parameters": [
                        {
                            "name": "SSID",
                            "id": "wifi_ssid",
                            "value": "Belategeuse_2.4"
                        },
                        {
                            "name": "Password",
                            "id": "wifi_pwd",
                            "value": "azertyuiop"
                        },
                        {
                            "name": "multicast DNS",
                            "id": "wifi_mdns",
                            "value": "piscine"
                        }
                    ]
                },
                {
                    "title": "Network Synchronized Time",
                    "parameters": [
                        {
                            "name": "NTP server",
                            "id": "ntp_server",
                            "value": "ntp.toto.fr"
                        },
                        {
                            "name": "Delay between Sync (ms)",
                            "id": "ntp_delay",
                            "value": "3600000"
                        }
                    ]
                }

            ]
        },
        {
            "id": "tp",
            "title": "Température piscine",
            "parameters": [
                {
                    "title": "Linear correction",
                    "parameters": [
                        {
                            "name": "Reference low",
                            "id": "refLow",
                            "value": "0"
                        },
                        {
                            "name": "Measured low",
                            "id": "rawLow",
                            "value": "0.49"
                        },
                        {
                            "name": "Reference high",
                            "id": "refHigh",
                            "value": "99.6"
                        },
                        {
                            "name": "Measured high",
                            "id": "rawHigh",
                            "value": "99.4"
                        }
                    ]
                },
                {
                    "title": "Multi sampling",
                    "parameters": [
                        {
                            "name": "Sampling number",
                            "id": "samples",
                            "value": "14"
                        },
                        {
                            "name": "Remove extrems",
                            "id": "extrems",
                            "value": "2"
                        },
                        {
                            "name": "Delay between samples (ms)",
                            "id": "delaySamples",
                            "value": "25"
                        },
                        {
                            "name": "Delay between measures (ms)",
                            "id": "delayMeasures",
                            "value": "60000"
                        }
                    ]
                }
            ]
        }
    ];

window.onload = function() {
    if (rzConfig && Array.isArray(rzConfig) && rzConfig.length > 0) {
        for (let i = 0; i < rzConfig.length; i++) {
            console.log("id=[" + rzConfig[i].id + "] title=[" + rzConfig[i].title + "]");
            document.getElementById("configContainer").insertAdjacentHTML("beforeend", htmlComponent(rzConfig[i]));
        }
    }
}

function htmlComponent(component) {
    return `
            <div class="siimple-card">
                <div class="siimple-card-header">
                    <div class="siimple-h3 siimple--mb-0">${component.title} (${component.id})</div>
                </div>
                <div class="siimple-card-body">
                    <form id="form-${component.id}" action="${config_api_url}" method="post">
                    ${component.parameters.map(p => htmlSubComponent(p)).join("")}
                    <div class="siimple-grid-row">
                        <div class="siimple-grid-col siimple-grid-col--12 siimple--py-0 siimple--text-right">
                            <input type="hidden" name="id" value="${component.id}"/>
                            <div class="siimple-btn siimple-btn--success" onclick="postForm('form#form-${component.id}');">Save changes</div>
                            <div class="siimple-btn siimple-btn--error" onclick="deleteForm('form#form-${component.id}');">Reset configuration</div>
                        </div>
                    </div>
                    </form>
                </div>
            </div>
        `;
}

function htmlSubComponent(subComponent) {
    console.log("htmlSubComponent ");
    return `
            <div class="siimple-grid-row siimple--ml-0">
                <div class="siimple-h5">${subComponent.title}</div>
                ${subComponent.parameters.map(p => htmlFieldSingleValue(p)).join("")}
            </div>
        `;

}

function htmlFieldSingleValue(parameter) {
    return `
            <div class="siimple-grid-col siimple-grid-col--3 siimple--py-0">
                <div class="siimple-field">
                    <div class="siimple-field-label">${parameter.name}</div>
                    <input name="${parameter.id}" type="text" class="siimple-input siimple-input--fluid" value="${parameter.value}">
                </div>
            </div>
        `;
}

function postForm(formSelector) {
    const formData = new FormData(document.querySelector(formSelector));

    return fetch(config_api_url, {
        method: 'POST', // 'GET', 'PUT', 'DELETE', etc.
        mode: 'cors',
        body: formData  // a FormData will automatically set the 'Content-Type'
    })
        .then(response => console.log(response))
        .catch(error => console.error(error));
}

function deleteForm(formSelector) {
    const formData = new FormData(document.querySelector(formSelector));

    return fetch(config_api_url, {
        method: 'DELETE', // 'GET', 'PUT', 'DELETE', etc.
        //mode: 'cors',
        body: formData  // a FormData will automatically set the 'Content-Type'
    })
        .then(response => console.log(response))
        .catch(error => console.error(error));
}

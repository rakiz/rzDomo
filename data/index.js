const serverAddress = "http://piscine"; // "http://192.168.1.56"; //

window.onload = function () {
    if (!window.fetch) {
        alert("Update your browser!");
    }
    moment.locale('fr');
    initCharts();
}

let unit2charts = {};
let id2dataset = {};
let id2config = {};

function initCharts() {
    const myHeaders = new Headers();
    const myInit = {method: 'GET', headers: myHeaders, mode: 'cors', cache: 'default'};

    const url = serverAddress + '/api/metrics?config';
    info(url);
    const myRequest = new Request(url, myInit);

    fetch(myRequest).then(function (response) {
        const contentType = response.headers.get("content-type");
        if (contentType && contentType.indexOf("application/json") !== -1) {
            response.json().then(function (json) {
                if (json && Array.isArray(json) && json.length > 0) { // if json is not an array, we have nothing to process
                    // [{"id":"tp","name":"Température Piscine","unit":"°C"}]
                    for (let i = 0; i < json.length; i++) {
                        let id = json[i].id;
                        id2config[id] = json[i];
                        let name = json[i].name;
                        let unit = json[i].unit;
                        // info("id=" + id + " name=" + name + " unit=" + unit);
                        let curChart = unit2charts[unit];
                        if (!curChart) {
                            info("Creating new graph for: " + name);
                            document.getElementById("graphContainer").insertAdjacentHTML("beforeend", htmlGraph(json[i]));
                            curChart = new Chart(document.getElementById('chart-' + unit), graphConf(json[i]));
                            unit2charts[unit] = curChart;
                            id2dataset[id] = curChart.data.datasets[0].data;
                        } else {
                            curChart.data.datasets.push({"data": []});
                            id2dataset[id] = curChart.data.datasets[curChart.data.datasets.length - 1].data;
                        }
                        document.getElementById("lastValuesContainer").insertAdjacentHTML("beforeend", htmlLastMetrics(json[i]));
                    }
                } else error("Nothing retrieved");
            });
        }
    }).then(function () {
        const updateInterval = 60 * 1000;
        updateData();
        setInterval(function () {
            updateData()
        }, updateInterval);

    });
}

let latestTimestamp = 0;

function updateData() {
    const myHeaders = new Headers();
    const myInit = {method: 'GET', headers: myHeaders, mode: 'cors', cache: 'default'};

    const url = serverAddress + '/api/metrics?since=' + latestTimestamp;
    info(url);
    const myRequest = new Request(url, myInit);

    fetch(myRequest).then(function (response) {
        const contentType = response.headers.get("content-type");
        if (contentType && contentType.indexOf("application/json") !== -1) {
            return response.json().then(function (json) {
                if (json && Array.isArray(json)) { // if json is not an array, we have nothing to process
                    if (json.length > 0) {
                        let lastValues = {};
                        for (let i = 0; i < json.length; i++) {
                            let time = json[i].ts;
                            for (const key of Object.keys(json[i])) {
                                if (key !== "ts") {
                                    lastValues[key] = {"time": time, "value": json[i][key]}; // { "tp" : { time: "jeudi", value : 18 } }
                                    add2Dataset(id2dataset[key], time, json[i][key]);
                                }
                            }
                        }
                        for (const key of Object.keys(unit2charts)) {
                            //console.log(key + " -> " + unit2charts[key])
                            unit2charts[key].update();
                        }
                        for (const key of Object.keys(lastValues)) {
                            document.getElementById("lastDate-" + key).textContent = moment.unix(lastValues[key].time).format('dddd Do MMMM YYYY, HH:mm:ss');
                            let conf = id2config[key];
                            document.getElementById("lastValue-" + key).textContent = conf.name + ": " + lastValues[key].value + conf.unit;
                        }
                    }
                } else error("Nothing retrieved");
            });
        } else {
            error("This is not the expected json: " + contentType);
        }
    });
}

function add2Dataset(dataset, ts, value) {
    const str = ts2str(ts);
    if (dataset.length === 0 || dataset[dataset.length - 1].x !== str) {
        info("ADDED value: " + str + " - " + value);
        latestTimestamp = latestTimestamp < ts ? ts : latestTimestamp;
        dataset.push({x: str, y: value})
    } else {
        info("IGNORED value: " + str + " - " + value);
    }
    if (dataset.length > 256) {
        dataset.shift();
    }
}

function resetZoom(unit) {
    unit2charts[unit].resetZoom();
}

function info(str) {
    if (console && console.info) {
        console.info(str);
    }
}

function error(str) {
    if (console && console.error) {
        console.error(str);
    }
}

function ts2str(timestamp) {
    return moment.unix(timestamp).format(); //"YYYY-mm-DDTHH:mm:ssZ")
}

function htmlGraph(input) {
    return `
            <div class="siimple-grid-col siimple-grid-col--12">
                <canvas id="chart-${input.unit}" height="100"></canvas>
                <div class="siimple--float-right siimple-btn siimple-btn--error  siimple-btn--small" onclick="resetZoom('${input.unit}')">Reset Chart</div>
            </div>
        `;
}

function htmlLastMetrics(input) {
    return `
            <div class="siimple-list-item siimple--clearfix">
                <div id="lastDate-${input.id}" class="siimple--float-left"></div>
                <div id="lastValue-${input.id}" class="siimple--float-right"></div>
            </div>
        `;
}

function graphConf(input) {
    return {
        type: 'line',
        data: {
            datasets: [{
                label: input.name, // 'Temperature piscine',
                fill: false,
                lineTension: 0.1,
                //data: [],
            }]
        },
        options: {
            responsive: true,
            title: {
                display: false
            },
            tooltips: {
                position: 'nearest',
                mode: 'index',
                intersect: false,
            },
            scales: {
                xAxes: [{
                    type: 'time',
                    display: true,
                    //time: {
                    //    unit: 'hour',
                    //},
                    scaleLabel: {
                        display: true,
                        labelString: 'Date'
                    },
                    ticks: {
                        major: {
                            fontStyle: 'bold',
                            fontColor: '#FF0000'
                        }
                    }
                }],
                yAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true,
                        labelString: input.unit, // '°C'
                    },
                    ticks: {
                        //suggestedMin: 10,
                        //suggestedMax: 30,

                        //min: 0,
                        //max: 50,

                        // forces step size to be 5 units
                        //stepSize: 1
                    }
                }]
            },
            plugins: {
                zoom: {
                    pan: {
                        enabled: true,
                        mode: 'x',
                        // On category scale, factor of pan velocity
                        speed: 20,
                        // Minimal pan distance required before actually applying pan
                        threshold: 10,
                    },
                    zoom: {
                        enabled: true,
                        drag: false,
                        mode: 'x',
                        speed: 0.05,
                    }
                }
            }
        }
    };
}
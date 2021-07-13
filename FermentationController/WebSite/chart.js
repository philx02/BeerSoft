
    window.chartColors = {
      red: 'rgb(255, 99, 132)',
      orange: 'rgb(255, 159, 64)',
      yellow: 'rgb(255, 205, 86)',
      green: 'rgb(75, 192, 192)',
      blue: 'rgb(54, 162, 235)',
      purple: 'rgb(153, 102, 255)',
      grey: 'rgb(231,233,237)'
    };

    Chart.defaults.global.defaultFontColor = 'white';

    datapointX = [];
    datapointWT = [];
    datapointAT = [];
    
    datapointX2 = [];
    datapointWT2 = [];
    datapointAT2 = [];

    var config1 = {
        type: 'line',
        data: {
            labels: datapointX,
            datasets: [{
              label: "Wort Temperature",
                data: datapointWT,
                borderColor: window.chartColors.blue,
                backgroundColor: 'rgba(0, 0, 0, 0)',
                fill: false,
                pointRadius: 0
            },{
              label: "Air Temperature",
                data: datapointAT,
                borderColor: window.chartColors.orange,
                backgroundColor: 'rgba(0, 0, 0, 0)',
                fill: false,
                pointRadius: 0
            }]
        },
        options: {
            responsive: true,
            title:{
                display:false
            },
            tooltips: {
                enabled: false
            },
            scales: {
                xAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true
                    },
                    gridLines: {
                        display: true,
                        color: 'rgba(255, 255, 255, 1)',
                        zeroLineColor: 'rgba(255, 255, 255, 1)'
                    }
                }],
                yAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true,
                        labelString: "Temperature"
                    },
                    gridLines: {
                        display: true,
                        color: 'rgba(255, 255, 255, 1)',
                        zeroLineColor: 'rgba(255, 255, 255, 1)'
                    }
                }]
            }
        }
    };

    var config2 = {
        type: 'line',
        data: {
            labels: datapointX2,
            datasets: [{
              label: "Wort Temperature",
                data: datapointWT2,
                borderColor: window.chartColors.blue,
                backgroundColor: 'rgba(0, 0, 0, 0)',
                fill: false,
                pointRadius: 0
            },{
              label: "Air Temperature",
                data: datapointAT2,
                borderColor: window.chartColors.orange,
                backgroundColor: 'rgba(0, 0, 0, 0)',
                fill: false,
                pointRadius: 0
            }]
        },
        options: {
            responsive: true,
            title:{
                display:false
            },
            tooltips: {
                enabled: false
            },
            scales: {
                xAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true
                    }
                }],
                yAxes: [{
                    display: true,
                    scaleLabel: {
                        display: true,
                        labelString: "Temperature"
                    }
                }]
            }
        }
    };

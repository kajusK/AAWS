/*
 * Weather station
 *
 * https://github.com/kajusK/aaws
 * Jakub Kaderka 2016
 */
function add_graph(div, data, label, unit)
{
	return new Dygraph(
		document.getElementById(div),
		data,
		{
			rollPeriod: 10,
			labels: ["date", label],
			showRangeSelector: true,
			labelsDivStyles: { 'textAlign': 'right' },
			ylabel: label + " [" + unit + "]",
			axes: {
				'y': {
					valueFormatter: function(y) {
						return y+" "+unit;
					}
				}
			}
		}
	);
}

function add_wind_graph(div, data, labels, units)
{
	labels.unshift("date");
	opts = {
		rollPeriod: 10,
		labels: labels,
		legend: "always",
		showRangeSelector: true,
		ylabel: labels[1] + " [" + units[0] + "]",
		y2label: labels[2] + " [" + units[1] + "]",
		series: {},
		axes: {
			'y2': {
				valueRange: [0,360],
				axisLabelFormatter: function(y2) {
					return y2.toPrecision(3);
				},
				valueFormatter: function(y2) {
					return y2+units[1];
				}
			},
			'y': {
				valueFormatter: function(y) {
					return y+" "+units[0];
				}
			}
		}
	};
	opts.series[labels[2]] = { axis: 'y2' };

	return new Dygraph(
		document.getElementById(div),
		data,
		opts
	);
}

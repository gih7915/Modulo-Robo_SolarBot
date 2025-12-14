let map;
let mapMarker;
let hasCentered = false;

function ensureMap() {
  if (map) return;
  map = L.map('map', { zoomControl: true }).setView([0, 0], 2);
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '© OpenStreetMap contribuintes',
    maxZoom: 19,
  }).addTo(map);
}

function fetchAll() {
  fetch('/api/all')
    .then(r => r.json())
    .then(data => {
      // Temperatura
      const tEl = document.getElementById('tempValue');
      if (data.temperature_ok && data.temperature !== null) {
        const prev = parseFloat(tEl.textContent);
        tEl.textContent = data.temperature.toFixed(2);
        const trendEl = document.getElementById('tempTrend');
        if (!isNaN(prev)) {
          const diff = data.temperature - prev;
          if (Math.abs(diff) < 0.01) {
            trendEl.textContent = 'Estável';
            trendEl.className = 'metric-trend';
          } else if (diff > 0) {
            trendEl.textContent = '+' + diff.toFixed(2) + '°C';
            trendEl.className = 'metric-trend trend-up';
          } else {
            trendEl.textContent = diff.toFixed(2) + '°C';
            trendEl.className = 'metric-trend trend-down';
          }
        }
      }
      
      // Pressão
      const pEl = document.getElementById('pressureValue');
      if (data.pressure !== null && data.pressure !== undefined) {
        const prev = parseFloat(pEl.textContent);
        pEl.textContent = data.pressure.toFixed(2);
        const trendEl = document.getElementById('pressureTrend');
        if (!isNaN(prev)) {
          const diff = data.pressure - prev;
          if (Math.abs(diff) < 0.1) {
            trendEl.textContent = 'Estável';
            trendEl.className = 'metric-trend';
          } else if (diff > 0) {
            trendEl.textContent = '+' + diff.toFixed(2) + ' hPa';
            trendEl.className = 'metric-trend trend-up';
          } else {
            trendEl.textContent = diff.toFixed(2) + ' hPa';
            trendEl.className = 'metric-trend trend-down';
          }
        }
      }
      
      // Altitude
      const aEl = document.getElementById('altitudeValue');
      if (data.altitude !== null && data.altitude !== undefined) {
        const prev = parseFloat(aEl.textContent);
        aEl.textContent = data.altitude.toFixed(1);
        const trendEl = document.getElementById('altitudeTrend');
        if (!isNaN(prev)) {
          const diff = data.altitude - prev;
          if (Math.abs(diff) < 0.5) {
            trendEl.textContent = 'Estável';
            trendEl.className = 'metric-trend';
          } else if (diff > 0) {
            trendEl.textContent = '+' + diff.toFixed(1) + ' m';
            trendEl.className = 'metric-trend trend-up';
          } else {
            trendEl.textContent = diff.toFixed(1) + ' m';
            trendEl.className = 'metric-trend trend-down';
          }
        }
      }
      
      // GPS
      const gps = data.gps || {};
      const hasCoords = (typeof gps.lat === 'number') && (typeof gps.lng === 'number');
      document.getElementById('gpsCoords').textContent = hasCoords ? (gps.lat.toFixed(6) + ', ' + gps.lng.toFixed(6)) : '--, --';
      const satVal = (typeof gps.satellites === 'number' && gps.satellites >= 0) ? gps.satellites : '--';
      document.getElementById('gpsSat').textContent = satVal;
      document.getElementById('gpsUTC').textContent = gps.utc || '--';
      document.getElementById('lastUpdate').textContent = data.timestamp || '--';

      if (hasCoords) {
        ensureMap();
        if (!mapMarker) {
          mapMarker = L.marker([gps.lat, gps.lng]).addTo(map);
        } else {
          mapMarker.setLatLng([gps.lat, gps.lng]);
        }
        if (!hasCentered) {
          map.setView([gps.lat, gps.lng], 15);
          hasCentered = true;
          setTimeout(() => map.invalidateSize(), 200);
        }
      }
      
      // MPU6050
      const mpu = data.mpu6050 || {};
      document.getElementById('accelX').textContent = mpu.accel_x !== undefined ? mpu.accel_x : '--';
      document.getElementById('accelY').textContent = mpu.accel_y !== undefined ? mpu.accel_y : '--';
      document.getElementById('accelZ').textContent = mpu.accel_z !== undefined ? mpu.accel_z : '--';
      document.getElementById('gyroX').textContent = mpu.gyro_x !== undefined ? mpu.gyro_x : '--';
      document.getElementById('gyroY').textContent = mpu.gyro_y !== undefined ? mpu.gyro_y : '--';
      document.getElementById('gyroZ').textContent = mpu.gyro_z !== undefined ? mpu.gyro_z : '--';
    })
    .catch(err => console.error('Erro fetch /api/all', err));
}

function loadHistory() {
  fetch('/export?format=csv')
    .then(r => r.text())
    .then(text => {
      const lines = text.trim().split('\n');
      if (lines.length <= 1) return; // only header
      const body = document.getElementById('historyBody');
      body.innerHTML = '';
      const header = lines[0].split(',');
      const rows = lines.slice(1).slice(-20); // last 20
      rows.forEach(line => {
        const cols = line.split(',');
        const tr = document.createElement('tr');
        // timestamp,temperature,pressure,lat,lng,satellites,fix
        for (let i = 0; i < cols.length; i++) {
          const td = document.createElement('td');
          const raw = cols[i];
          const val = (raw === undefined || raw === null || raw.trim() === '') ? '--' : raw;
          td.textContent = val;
          tr.appendChild(td);
        }
        body.appendChild(tr);
      });
    })
    .catch(err => console.error('Erro carregar histórico', err));
}

function setupExportButtons() {
  const btnCSV = document.getElementById('btnExportCSV');
  const btnJSON = document.getElementById('btnExportJSON');
  btnCSV.addEventListener('click', () => {
    window.location.href = '/export?format=csv';
  });
  btnJSON.addEventListener('click', () => {
    window.location.href = '/export?format=json';
  });
}

// Inicialização
document.addEventListener('DOMContentLoaded', () => {
  setupExportButtons();
  fetchAll();
  loadHistory();
  setInterval(fetchAll, 3000);
  setInterval(loadHistory, 7000);
});

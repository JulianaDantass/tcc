import folium
from folium.plugins import MarkerCluster

lendo_primeira_variavel = True
nos_originais = []
nos_aproximados = []

with open('coordenadas.txt', 'r') as arquivo:
    for linha in arquivo:
        linha_limpa = linha.strip()
        
        if linha_limpa == "":
            lendo_primeira_variavel = False
            continue
            
        if lendo_primeira_variavel:
            nos_originais.append(linha_limpa)
        else:
            nos_aproximados.append(linha_limpa)


lons_nos_originais = []
lats_nos_originais = []

for linha_texto in nos_originais:
    
    texto_limpo = linha_texto.replace('(', '').replace(')', '')
    partes = texto_limpo.split(',')
    lat = float(partes[0])
    lon = float(partes[1])
    lats_nos_originais.append(lat)
    lons_nos_originais.append(lon)


# 1. Encontrar o ponto central matemático para o mapa abrir focado nas casas
centro_lat = sum(lats_nos_originais) / len(lats_nos_originais)
centro_lon = sum(lons_nos_originais) / len(lons_nos_originais)

# 2. Criar o mapa base
mapa_interativo = folium.Map(location=[centro_lat, centro_lon], zoom_start=15)

# 3. Criar a "camada" de Cluster e adicioná-la ao mapa
cluster = MarkerCluster(
    name="Casas dos Pacientes",
    maxClusterRadius=30,
    
).add_to(mapa_interativo)

# 4. Loop pelas 307 coordenadas usando zip() para ler as duas listas ao mesmo tempo
for lat, lon in zip(lats_nos_originais, lons_nos_originais):
    
    # Criamos o marcador e adicionamos ao CLUSTER, não ao mapa direto
    folium.CircleMarker(
        location=[lat, lon],
        radius=6,
        color='blue',
        fill=True,
        fill_color='blue',
        fill_opacity=0.7,
        popup=f"Coordenada: {lat}, {lon}" # Ao clicar na bolinha, mostra o texto
    ).add_to(cluster)

# 5. Salvar o arquivo
mapa_interativo.save("mapa_cluster_torre.html")
print("Mapa salvo!")
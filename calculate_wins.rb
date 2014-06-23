results = nil
total = 0

ARGF.each_line do |line|
  values = line.split(',')
  results ||= Array.new(values.size/2,0)
  values = values[0...results.size]

  best = 0
  values.each_with_index do |x,i|
    best = i if x.to_f < values[best].to_f
  end

  results[best] += 1
  total += 1
end

puts "#{results.join(', ')}, #{total}"

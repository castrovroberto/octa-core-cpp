#!/usr/bin/env python3
"""
Benchmark Regression Detection Tool
Phase P2.3.1 - CI Pipeline Integration

Compares benchmark results against baseline to detect performance regressions.
"""

import json
import sys
import argparse
from typing import Dict, List, Tuple, Optional
import statistics


class BenchmarkComparator:
    """Compare benchmark results and detect regressions."""
    
    # Regression thresholds from benchmark validation report
    REGRESSION_THRESHOLDS = {
        'core_latency_warning': 0.10,      # 10% increase
        'core_latency_critical': 0.25,     # 25% increase
        'safety_overhead_warning': 0.05,   # 5% increase
        'safety_overhead_critical': 0.15,  # 15% increase
        'throughput_warning': 0.15,        # 15% decrease
        'throughput_critical': 0.30,       # 30% decrease
        'memory_warning': 0.20,            # 20% increase
        'memory_critical': 0.50,           # 50% increase
    }
    
    def __init__(self, baseline_file: str, current_file: str):
        """Initialize comparator with baseline and current results."""
        self.baseline_file = baseline_file
        self.current_file = current_file
        self.baseline_data = self._load_json(baseline_file)
        self.current_data = self._load_json(current_file)
        self.warnings = []
        self.critical_issues = []
        
    def _load_json(self, filename: str) -> Dict:
        """Load JSON benchmark results."""
        try:
            with open(filename, 'r') as f:
                data = json.load(f)
                if 'benchmarks' in data:
                    return data
                else:
                    # Handle older format
                    return {'benchmarks': data}
        except FileNotFoundError:
            print(f"Error: File {filename} not found")
            sys.exit(1)
        except json.JSONDecodeError:
            print(f"Error: Invalid JSON in {filename}")
            sys.exit(1)
    
    def _get_benchmark_by_name(self, benchmarks: List[Dict], name: str) -> Optional[Dict]:
        """Find benchmark by name in results."""
        for bench in benchmarks:
            if bench.get('name') == name:
                return bench
        return None
    
    def _calculate_percentage_change(self, baseline: float, current: float) -> float:
        """Calculate percentage change from baseline to current."""
        if baseline == 0:
            return 0.0
        return (current - baseline) / baseline
    
    def _format_time(self, time_us: float) -> str:
        """Format time with appropriate units."""
        if time_us < 1:
            return f"{time_us * 1000:.2f} ns"
        elif time_us < 1000:
            return f"{time_us:.2f} μs"
        else:
            return f"{time_us / 1000:.2f} ms"
    
    def compare_core_performance(self) -> Dict:
        """Compare core makeMove performance benchmarks."""
        core_benchmarks = [
            'BM_MakeMove_ShortChain_mean',
            'BM_MakeMove_MediumChain_mean',
            'BM_MakeMove_LongChain_mean'
        ]
        
        results = {}
        
        for bench_name in core_benchmarks:
            baseline = self._get_benchmark_by_name(self.baseline_data['benchmarks'], bench_name)
            current = self._get_benchmark_by_name(self.current_data['benchmarks'], bench_name)
            
            if baseline and current:
                baseline_time = baseline.get('cpu_time', 0)
                current_time = current.get('cpu_time', 0)
                change = self._calculate_percentage_change(baseline_time, current_time)
                
                results[bench_name] = {
                    'baseline_time': baseline_time,
                    'current_time': current_time,
                    'change_percent': change * 100,
                    'status': self._assess_performance_change(change, 'core_latency')
                }
                
                # Check thresholds
                if change > self.REGRESSION_THRESHOLDS['core_latency_critical']:
                    self.critical_issues.append(
                        f"CRITICAL: {bench_name} increased by {change*100:.1f}% "
                        f"({self._format_time(baseline_time)} → {self._format_time(current_time)})"
                    )
                elif change > self.REGRESSION_THRESHOLDS['core_latency_warning']:
                    self.warnings.append(
                        f"WARNING: {bench_name} increased by {change*100:.1f}% "
                        f"({self._format_time(baseline_time)} → {self._format_time(current_time)})"
                    )
        
        return results
    
    def compare_safety_overhead(self) -> Dict:
        """Compare safety system overhead."""
        validate_only = self._get_benchmark_by_name(
            self.baseline_data['benchmarks'], 'BM_SafetyLevel_Comparison/0_mean'
        )
        light_undo_baseline = self._get_benchmark_by_name(
            self.baseline_data['benchmarks'], 'BM_SafetyLevel_Comparison/1_mean'
        )
        
        validate_only_current = self._get_benchmark_by_name(
            self.current_data['benchmarks'], 'BM_SafetyLevel_Comparison/0_mean'
        )
        light_undo_current = self._get_benchmark_by_name(
            self.current_data['benchmarks'], 'BM_SafetyLevel_Comparison/1_mean'
        )
        
        if all([validate_only, light_undo_baseline, validate_only_current, light_undo_current]):
            # Calculate overhead for baseline and current
            baseline_overhead = self._calculate_percentage_change(
                validate_only['cpu_time'], light_undo_baseline['cpu_time']
            )
            current_overhead = self._calculate_percentage_change(
                validate_only_current['cpu_time'], light_undo_current['cpu_time']
            )
            
            overhead_change = current_overhead - baseline_overhead
            
            result = {
                'baseline_overhead_percent': baseline_overhead * 100,
                'current_overhead_percent': current_overhead * 100,
                'overhead_change_percent': overhead_change * 100,
                'status': self._assess_performance_change(overhead_change, 'safety_overhead')
            }
            
            # Check thresholds
            if overhead_change > self.REGRESSION_THRESHOLDS['safety_overhead_critical']:
                self.critical_issues.append(
                    f"CRITICAL: Safety overhead increased by {overhead_change*100:.1f}% "
                    f"({baseline_overhead*100:.1f}% → {current_overhead*100:.1f}%)"
                )
            elif overhead_change > self.REGRESSION_THRESHOLDS['safety_overhead_warning']:
                self.warnings.append(
                    f"WARNING: Safety overhead increased by {overhead_change*100:.1f}% "
                    f"({baseline_overhead*100:.1f}% → {current_overhead*100:.1f}%)"
                )
            
            return result
        
        return {}
    
    def compare_memory_performance(self) -> Dict:
        """Compare memory-related benchmarks."""
        memory_benchmarks = [
            'BM_MakeMove_MemoryTracking/64_mean',
            'BM_MakeMove_MemoryTracking/256_mean'
        ]
        
        results = {}
        
        for bench_name in memory_benchmarks:
            baseline = self._get_benchmark_by_name(self.baseline_data['benchmarks'], bench_name)
            current = self._get_benchmark_by_name(self.current_data['benchmarks'], bench_name)
            
            if baseline and current:
                baseline_throughput = baseline.get('bytes_per_second', 0)
                current_throughput = current.get('bytes_per_second', 0)
                
                # For throughput, negative change is bad (decrease in performance)
                change = self._calculate_percentage_change(baseline_throughput, current_throughput)
                
                results[bench_name] = {
                    'baseline_throughput': baseline_throughput,
                    'current_throughput': current_throughput,
                    'change_percent': change * 100,
                    'status': self._assess_throughput_change(change)
                }
                
                # Check thresholds (negative change is bad for throughput)
                if change < -self.REGRESSION_THRESHOLDS['throughput_critical']:
                    self.critical_issues.append(
                        f"CRITICAL: {bench_name} throughput decreased by {abs(change)*100:.1f}%"
                    )
                elif change < -self.REGRESSION_THRESHOLDS['throughput_warning']:
                    self.warnings.append(
                        f"WARNING: {bench_name} throughput decreased by {abs(change)*100:.1f}%"
                    )
        
        return results
    
    def _assess_performance_change(self, change: float, category: str) -> str:
        """Assess performance change severity."""
        warning_threshold = self.REGRESSION_THRESHOLDS[f'{category}_warning']
        critical_threshold = self.REGRESSION_THRESHOLDS[f'{category}_critical']
        
        if change > critical_threshold:
            return 'CRITICAL'
        elif change > warning_threshold:
            return 'WARNING'
        elif change < -0.05:  # 5% improvement
            return 'IMPROVEMENT'
        else:
            return 'OK'
    
    def _assess_throughput_change(self, change: float) -> str:
        """Assess throughput change (negative is bad)."""
        if change < -self.REGRESSION_THRESHOLDS['throughput_critical']:
            return 'CRITICAL'
        elif change < -self.REGRESSION_THRESHOLDS['throughput_warning']:
            return 'WARNING'
        elif change > 0.05:  # 5% improvement
            return 'IMPROVEMENT'
        else:
            return 'OK'
    
    def generate_report(self) -> str:
        """Generate comprehensive comparison report."""
        report = []
        report.append("=" * 60)
        report.append("BENCHMARK REGRESSION ANALYSIS REPORT")
        report.append("=" * 60)
        report.append(f"Baseline: {self.baseline_file}")
        report.append(f"Current:  {self.current_file}")
        report.append("")
        
        # Core performance analysis
        report.append("CORE PERFORMANCE ANALYSIS")
        report.append("-" * 40)
        core_results = self.compare_core_performance()
        for bench_name, data in core_results.items():
            clean_name = bench_name.replace('BM_MakeMove_', '').replace('_mean', '')
            report.append(f"{clean_name:15s}: {self._format_time(data['baseline_time'])} → "
                         f"{self._format_time(data['current_time'])} "
                         f"({data['change_percent']:+.1f}%) [{data['status']}]")
        report.append("")
        
        # Safety overhead analysis
        report.append("SAFETY SYSTEM OVERHEAD ANALYSIS")
        report.append("-" * 40)
        safety_results = self.compare_safety_overhead()
        if safety_results:
            report.append(f"Baseline overhead: {safety_results['baseline_overhead_percent']:.2f}%")
            report.append(f"Current overhead:  {safety_results['current_overhead_percent']:.2f}%")
            report.append(f"Change:           {safety_results['overhead_change_percent']:+.2f}% "
                         f"[{safety_results['status']}]")
        else:
            report.append("Safety overhead data not available")
        report.append("")
        
        # Memory performance analysis
        report.append("MEMORY PERFORMANCE ANALYSIS")
        report.append("-" * 40)
        memory_results = self.compare_memory_performance()
        for bench_name, data in memory_results.items():
            clean_name = bench_name.replace('BM_MakeMove_MemoryTracking/', '').replace('_mean', '')
            report.append(f"Chain {clean_name:3s}: {data['change_percent']:+.1f}% throughput change "
                         f"[{data['status']}]")
        report.append("")
        
        # Summary
        report.append("SUMMARY")
        report.append("-" * 40)
        if self.critical_issues:
            report.append("❌ CRITICAL ISSUES DETECTED:")
            for issue in self.critical_issues:
                report.append(f"   {issue}")
        
        if self.warnings:
            report.append("⚠️  WARNINGS:")
            for warning in self.warnings:
                report.append(f"   {warning}")
        
        if not self.critical_issues and not self.warnings:
            report.append("✅ No performance regressions detected")
        
        report.append("")
        report.append("=" * 60)
        
        return "\n".join(report)
    
    def get_exit_code(self) -> int:
        """Get appropriate exit code for CI."""
        if self.critical_issues:
            return 2  # Critical issues
        elif self.warnings:
            return 1  # Warnings only
        else:
            return 0  # All good


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Compare benchmark results and detect performance regressions"
    )
    parser.add_argument("baseline", help="Baseline benchmark results (JSON)")
    parser.add_argument("current", help="Current benchmark results (JSON)")
    parser.add_argument("--output", "-o", help="Output report to file")
    parser.add_argument("--exit-on-regression", action="store_true",
                       help="Exit with non-zero code if regressions detected")
    
    args = parser.parse_args()
    
    # Perform comparison
    comparator = BenchmarkComparator(args.baseline, args.current)
    report = comparator.generate_report()
    
    # Output report
    if args.output:
        with open(args.output, 'w') as f:
            f.write(report)
        print(f"Report written to {args.output}")
    else:
        print(report)
    
    # Exit with appropriate code
    if args.exit_on_regression:
        exit_code = comparator.get_exit_code()
        if exit_code != 0:
            print(f"\nExiting with code {exit_code} due to performance issues")
        sys.exit(exit_code)


if __name__ == "__main__":
    main() 